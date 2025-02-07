#include "hashi/include/dir.hpp"
#include "hashi/include/stream.hpp"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

// helper functions
template <typename T> void emplaceBytes(const T &t, std::vector<char> &buffer) {
  for (unsigned long i = 0; i < sizeof(T); ++i) {
    buffer.emplace_back(static_cast<char>(t >> (i * 8)));
  }
}

// mask the `number` to make only `num_bits` LSBs with the original value. The
// other bits are changed to 0.
template <typename T> T pickLowBits(const T &number, const T &num_bits) {
  return ~(std::numeric_limits<T>::max() << num_bits) & number;
}

const size_t MAX_BUCKET_REGS = 3;
const unsigned long DEEP_SIZE = sizeof(unsigned short);
const unsigned long BUCKET_PTR_SIZE = sizeof(unsigned int);
// local deep + bucket pointer
const unsigned long BUCKET_REF_SIZE = DEEP_SIZE + BUCKET_PTR_SIZE;

HashiDir::HashiDir(const std::string &db_path, const std::string &table_name,
                   unsigned short gd)
    : hashd_file{db_path + "/" + table_name + "_dir.bin"},
      bucket_dir{db_path + "/" + table_name + "_buckets"} {
  const unsigned short DEFAULT_GD = gd;
  const unsigned short DEFAULT_LD = 2;

  if (!std::filesystem::exists(hashd_file)) {
    std::ofstream file(hashd_file, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to create the hash directory");
    }

    unsigned int n_buckets = static_cast<unsigned int>(
        std::pow(2.0, static_cast<double>(DEFAULT_GD)));
    size_t n_bytes = DEEP_SIZE + n_buckets * (BUCKET_REF_SIZE);

    // bytes in the format: global_deep | [<local_deep,bucket_ref>, ...]
    std::vector<char> bytes;
    bytes.reserve(n_bytes);

    // add global deep
    global_deep = DEFAULT_GD;
    emplaceBytes(DEFAULT_GD, bytes);

    for (unsigned int i = 0; i < n_buckets; ++i) {
      // add local_deep
      emplaceBytes(DEFAULT_LD, bytes);
      // add bucket_ref (pick only DEFAULT_LD bits)
      emplaceBytes(pickLowBits(i, static_cast<unsigned int>(DEFAULT_LD)),
                   bytes);
    }
    file.write(&bytes[0], bytes.size());
    file.close();
  } else {
    std::ifstream file(hashd_file, std::ios::binary);
    if (!file.is_open()) {
      throw std::runtime_error("Failed to read the hash directory");
    }
    file.read(reinterpret_cast<char *>(&global_deep), sizeof(global_deep));
  }

  if (!std::filesystem::exists(bucket_dir)) {
    if (!std::filesystem::create_directory(bucket_dir)) {
      throw std::runtime_error("Failed to create bucket directory!");
    }
  }
}

unsigned short HashiDir::get_global_deep() const { return global_deep; }

void HashiDir::flush_global_deep() const {
  std::fstream dir_file{hashd_file,
                        std::ios::in | std::ios::out | std::ios::binary};
  if (!dir_file.is_open()) {
    throw std::runtime_error(
        "Failed to open the directory file, context: flush_global_deep");
  }
  dir_file.seekp(0, std::ios::beg);
  dir_file.write(reinterpret_cast<const char *>(&global_deep),
                 sizeof(global_deep));
}

void HashiDir::flush_bucket(const Bucket &b) const {
  std::fstream dir_file{hashd_file,
                        std::ios::in | std::ios::out | std::ios::binary};
  if (!dir_file.is_open()) {
    throw std::runtime_error(
        "Failed to open the directory file, context: flush_global_deep");
  }
  dir_file.seekp(DEEP_SIZE + b.get_key() * BUCKET_REF_SIZE, std::ios::beg);
  unsigned short ld = b.get_local_deep();
  dir_file.write(reinterpret_cast<const char *>(&ld), DEEP_SIZE);
  unsigned int key = b.get_key();
  dir_file.write(reinterpret_cast<const char *>(&key), BUCKET_PTR_SIZE);
}

Bucket HashiDir::get_bucket(const unsigned int &rid) const {
  std::ifstream ifile{hashd_file, std::ios::binary};

  if (!ifile.is_open()) {
    throw std::runtime_error("Failed to open the bucket directory " +
                             hashd_file);
  }

  unsigned short gd;
  ifile.read(reinterpret_cast<char *>(&gd), sizeof(gd));
  auto reg_idx = pickLowBits(rid, static_cast<unsigned int>(gd));

  // jump to bucket_ref index
  ifile.seekg(reg_idx * (BUCKET_REF_SIZE), std::ios::cur);

  unsigned short ld;
  unsigned int bucket_key;

  ifile.read(reinterpret_cast<char *>(&ld), DEEP_SIZE);
  ifile.read(reinterpret_cast<char *>(&bucket_key), BUCKET_PTR_SIZE);
  ifile.close();

  return Bucket{ld, bucket_key, bucket_dir};
}

void HashiDir::duplicate_dir() {
  std::ifstream dir_ifs{hashd_file, std::ios::binary};
  if (!dir_ifs.is_open()) {
    throw std::runtime_error("Could not open hash directory file for write");
  }

  std::ofstream dir_ofs{hashd_file, std::ios::binary | std::ios::app};
  if (!dir_ofs.is_open()) {
    throw std::runtime_error("Could not open hash directory file for write");
  }

  unsigned short ld;
  unsigned int key;
  auto dir_slots = static_cast<unsigned long>(std::pow(2, global_deep));

  // skip global deep
  dir_ifs.seekg(DEEP_SIZE, std::ios::beg);
  for (unsigned long i = 0; i < dir_slots; ++i) {
    dir_ifs.read(reinterpret_cast<char *>(&ld), sizeof(ld));
    dir_ifs.read(reinterpret_cast<char *>(&key), sizeof(key));

    dir_ofs.write(reinterpret_cast<const char *>(&ld), sizeof(ld));
    dir_ofs.write(reinterpret_cast<const char *>(&key), sizeof(key));
  }
  dir_ifs.close();
  dir_ofs.close();
  ++global_deep;
  flush_global_deep();
}

Bucket HashiDir::split_bucket(Bucket &b) {
  std::fstream bucket_file = b.get_fstream(std::ios::in);
  if (!bucket_file.is_open()) {
    throw std::runtime_error("Failed to open the bucket file");
  }

  std::string bucket_cache_path = b.get_path() + ".cache";
  std::ofstream bucket_cache_ofs{bucket_cache_path};

  if (!bucket_cache_ofs.is_open()) {
    throw std::runtime_error("Could not open bucket cache file");
  }

  unsigned int new_bucket_key = (1 << b.get_local_deep()) | b.get_key();
  b.incr_local_deep();
  Bucket new_bucket{b.get_local_deep(), new_bucket_key, bucket_dir};
  std::fstream new_bucket_ofs =
      new_bucket.get_fstream(std::ios::out | std::ios::app);

  if (!new_bucket_ofs.is_open()) {
    throw std::runtime_error("Could not open new bucket file");
  }

  Reg r;
  std::string reg_string;
  unsigned int ld{static_cast<unsigned int>(b.get_local_deep())};
  while (std::getline(bucket_file, reg_string)) {
    r = parseCsv(reg_string);
    if (pickLowBits(r.get_id(), ld) == new_bucket_key) {
      new_bucket_ofs << reg_string << "\n";
    } else {
      bucket_cache_ofs << reg_string << "\n";
    }
  }
  bucket_cache_ofs.close();
  new_bucket_ofs.close();
  bucket_file.close();

  std::filesystem::rename(bucket_cache_path, b.get_path());

  if (b.get_local_deep() > global_deep) {
    duplicate_dir();
  }
  flush_bucket(b);
  flush_bucket(new_bucket);
  return new_bucket;
}

void HashiDir::add_into_bucket(Bucket &b, const Reg &r) {
  if (!std::filesystem::exists(b.get_path())) {
    // create empty bucket file
    std::fstream bucket_ofs = b.get_fstream(std::ios::out);
    bucket_ofs.close();
  }
  std::fstream bucket_ifs = b.get_fstream(std::ios::in);
  if (!bucket_ifs.is_open()) {
    throw std::runtime_error(
        "Failed to open the bucket file, context: add_into_bucket");
  }

  // count number of registers that are already in the bucket
  size_t reg_count = 0;
  std::string tmp;
  while (std::getline(bucket_ifs, tmp)) {
    ++reg_count;
  }
  bucket_ifs.close();

  if (reg_count < MAX_BUCKET_REGS) {
    // append register to the bucket
    std::fstream bucket_ofs = b.get_fstream(std::ios::out | std::ios::app);

    if (!bucket_ofs.is_open()) {
      throw std::runtime_error("Failed to open table file");
    }
    bucket_ofs << regToCsv(r) << "\n";
    bucket_ofs.close();
  } else if (reg_count == MAX_BUCKET_REGS) {
    Bucket new_bucket = split_bucket(b);
    auto ld = static_cast<unsigned int>(b.get_local_deep());

    // NOTE: posibilty of recursion never end only if all ids are equal.
    // Therefore r.get_id() must be a unique key.
    if (pickLowBits(r.get_id(), ld) == new_bucket.get_key()) {
      add_into_bucket(new_bucket, r);
    } else {
      add_into_bucket(b, r);
    }
  } else {
    std::overflow_error("Max bucket size overflow. Expected max of " +
                        std::to_string(MAX_BUCKET_REGS) + ", found " +
                        std::to_string(reg_count));
  }
}

void HashiDir::add_reg(const Reg &r) {
  Bucket b = get_bucket(r.get_id());
  add_into_bucket(b, r);
}

IncLog HashiDir::add_reg_and_log(const Reg &r) {
  Bucket b = get_bucket(r.get_id());
  add_into_bucket(b, r);
  return IncLog{global_deep, b.get_local_deep()};
}

unsigned int HashiDir::del_from_bucket(const Bucket &b,
                                       const unsigned int &rid) const {
  if (!std::filesystem::exists(b.get_path())) {
    return 0;
  }
  std::fstream bucket_ifs = b.get_fstream(std::ios::in);
  if (!bucket_ifs.is_open()) {
    throw std::runtime_error(
        "Failed to open the bucket file, context: del_from_bucket");
  }

  std::string bucket_cache_path = b.get_path() + ".cache";
  std::ofstream bucket_cache_ofs{bucket_cache_path};

  if (!bucket_cache_ofs.is_open()) {
    throw std::runtime_error("Could not open bucket cache file");
  }

  Reg r;
  std::string reg_string;
  unsigned int count = 0;
  while (std::getline(bucket_ifs, reg_string)) {
    r = parseCsv(reg_string);
    if (r.get_id() != rid) {
      bucket_cache_ofs << reg_string << '\n';
    } else {
      ++count;
    }
  }
  bucket_ifs.close();
  std::filesystem::rename(bucket_cache_path, b.get_path());
  return count;
}

void HashiDir::del_reg(const unsigned int &rid) const {
  del_from_bucket(get_bucket(rid), rid);
}

RemLog HashiDir::del_reg_and_log(const unsigned int &rid) const {
  Bucket b = get_bucket(rid);
  unsigned int removed = del_from_bucket(b, rid);
  return RemLog{
      removed,
      global_deep,
      b.get_local_deep(),
  };
}

std::vector<Reg> HashiDir::get_reg(const unsigned int &rid) const {
  Bucket b = get_bucket(rid);
  std::fstream bucket_ifs = b.get_fstream(std::ios::in);
  if (!bucket_ifs.is_open()) {
    throw std::runtime_error("Failed to open the bucket file");
  }
  std::vector<Reg> results;

  std::string line;
  unsigned int id;
  while (std::getline(bucket_ifs, line)) {
    id = parseCsv(line).get_id();
    if (id == rid) {
      results.emplace_back(parseCsv(line));
    }
  }
  bucket_ifs.close();
  return results;
}

BusLog HashiDir::get_reg_and_log(const unsigned int &rid) const {
  std::vector<Reg> rs = get_reg(rid);
  return BusLog{static_cast<unsigned int>(rs.size())};
}
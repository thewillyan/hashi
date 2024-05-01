#include "hashi/include/dir.hpp"
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

HashiDir::HashiDir(const std::string &db_path, const std::string &table_name)
    : hashd_file{db_path + "/" + table_name + "_dir.bin"},
      bucket_dir{db_path + "/" + table_name + "_buckets"} {
  const unsigned short DEFAULT_GD = 3;
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

void HashiDir::flush_global_deep() const {
  std::fstream dir_file{hashd_file, std::ios::out};
  if (!dir_file.is_open()) {
    throw std::runtime_error(
        "Failed to open the directory file, context: flush_global_deep");
  }
  dir_file.seekp(0, std::ios::beg);
  dir_file.write(reinterpret_cast<char *>(global_deep), sizeof(global_deep));
}

void HashiDir::flush_local_deep(const Bucket &b) const {
  std::fstream dir_file{hashd_file, std::ios::out};
  if (!dir_file.is_open()) {
    throw std::runtime_error(
        "Failed to open the directory file, context: flush_global_deep");
  }
  dir_file.seekp(DEEP_SIZE + b.get_key() * BUCKET_REF_SIZE, std::ios::beg);
  dir_file.write(reinterpret_cast<char *>(b.get_local_deep()), DEEP_SIZE);
}

Bucket HashiDir::split_bucket(Bucket &b) {
  std::fstream bucket_file = b.get_fstream(std::ios::in);
  if (!bucket_file.is_open()) {
    throw std::runtime_error("Failed to open the bucket file");
  }
  RegStream rs{bucket_file};

  std::string bucket_path = b.get_path();
  std::string bucket_cache_path = bucket_path + "_cache";
  std::ofstream bucket_cache_ofs{bucket_cache_path};

  if (!bucket_cache_ofs.is_open()) {
    throw std::runtime_error("Could not open bucket cache file");
  }

  b.incr_local_deep();
  unsigned int new_bucket_key = (1 << b.get_local_deep()) | b.get_key();
  Bucket new_bucket{b.get_local_deep(), new_bucket_key, bucket_dir};
  std::fstream new_bucket_ofs =
      new_bucket.get_fstream(std::ios::out | std::ios::app);

  if (!new_bucket_ofs.is_open()) {
    throw std::runtime_error("Could not open new bucket file");
  }

  Reg r;
  unsigned int ld{static_cast<unsigned int>(b.get_local_deep())};
  while (!rs.endOfStream()) {
    rs >> r;
    if (pickLowBits(r.get_id(), ld) == new_bucket_key) {
      new_bucket_ofs << regToCsv(r) << "\n";
    } else {
      bucket_cache_ofs << regToCsv(r) << "\n";
    }
  }
  bucket_cache_ofs.close();
  new_bucket_ofs.close();
  bucket_file.close();

  std::filesystem::remove(bucket_path);
  std::filesystem::rename(bucket_cache_path, bucket_path);

  if (b.get_local_deep() > global_deep) {
    duplicate_dir();
  }
  flush_local_deep(b);
  flush_local_deep(new_bucket);
  return new_bucket;
}

void HashiDir::add_into_bucket(Bucket &b, const Reg &r) {
  std::fstream bucket_ifs = b.get_fstream(std::ios::in);
  if (!bucket_ifs.is_open()) {
    throw std::runtime_error("Failed toopen the bucket file!");
  }
  auto rstream = RegStream(bucket_ifs);

  // count number of registers that are already in the bucket
  size_t reg_count = 0;
  while (!rstream.endOfStream()) {
    rstream.skip(1);
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

void HashiDir::duplicate_dir() {
  std::fstream dir_file{hashd_file,
                        std::ios::in | std::ios::out | std::ios::app};
  if (!dir_file.is_open()) {
    throw std::runtime_error("Could not open hash directory file");
  }

  unsigned short ld;
  unsigned int key;
  auto dir_slots = static_cast<unsigned long>(std::pow(2, global_deep));

  // skip global deep
  dir_file.seekg(DEEP_SIZE, std::ios::beg);
  unsigned int mask = (1 << global_deep);
  for (unsigned long i = 0; i < dir_slots; ++i) {
    dir_file.read(reinterpret_cast<char *>(&ld), sizeof(ld));
    dir_file.read(reinterpret_cast<char *>(&key), sizeof(key));
    unsigned int new_key = mask | key;

    dir_file.write(reinterpret_cast<const char *>(&ld), sizeof(ld));
    dir_file.write(reinterpret_cast<const char *>(&new_key), sizeof(new_key));
  }
  dir_slots *= 2;
  dir_file.close();
  ++global_deep;
  flush_global_deep();
}

void HashiDir::add_reg(const Reg &r) {
  std::ifstream ifile{hashd_file, std::ios::binary};

  if (!ifile.is_open()) {
    throw std::runtime_error("Failed to open the bucket directory " +
                             hashd_file);
  }

  unsigned short gd;
  ifile.read(reinterpret_cast<char *>(&gd), sizeof(gd));
  auto reg_idx = pickLowBits(r.get_id(), static_cast<unsigned int>(gd));

  // jump to bucket_ref index
  ifile.seekg(reg_idx * (BUCKET_REF_SIZE), std::ios::cur);

  unsigned short ld;
  unsigned int bucket_key;

  ifile.read(reinterpret_cast<char *>(&ld), DEEP_SIZE);
  ifile.read(reinterpret_cast<char *>(&bucket_key), BUCKET_PTR_SIZE);
  ifile.close();

  Bucket b{ld, bucket_key, bucket_dir};
  add_into_bucket(b, r);
}

void HashiDir::del_reg(const unsigned int &r_key) const {}

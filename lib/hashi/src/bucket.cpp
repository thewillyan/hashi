#include "hashi/include/bucket.hpp"
#include <fstream>

Bucket::Bucket(unsigned short local_deep, unsigned int k,
               std::string bucket_dir)
    : ld{local_deep}, key{k},
      path{bucket_dir + "/" + std::to_string(k) + ".csv"} {}

// getters
unsigned short Bucket::get_local_deep() const { return ld; }
unsigned int Bucket::get_key() const { return key; }
const std::string &Bucket::get_path() const { return path; }

void Bucket::incr_local_deep() { ++ld; }

std::fstream Bucket::get_fstream(const std::ios::openmode &flags) const {
  return std::fstream{path, flags};
}

RegStream Bucket::get_regstream() const {
  auto stream = get_fstream(std::ios::in);
  return RegStream(stream);
}

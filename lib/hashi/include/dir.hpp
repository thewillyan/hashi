#ifndef HASHI_DIR
#define HASHI_DIR

#include "hashi/include/bucket.hpp"
#include "hashi/include/reg.hpp"
#include <optional>
#include <string>

// maybe should be called "HashDir"
class HashiDir {
private:
  unsigned short global_deep;
  const std::string hashd_file;
  const std::string bucket_dir;

  // flushes
  void flush_global_deep() const;
  void flush_bucket(const Bucket &b) const;

  Bucket get_bucket(const unsigned int &rid) const;

  // split a bucket into two
  Bucket split_bucket(Bucket &b);

  void add_into_bucket(Bucket &b, const Reg &r);
  void duplicate_dir();

  void del_from_bucket(const Bucket &b, const unsigned int &rid) const;

public:
  HashiDir(const std::string &db_path, const std::string &table_name);

  void add_reg(const Reg &r);
  void del_reg(const unsigned int &rid) const;
  std::optional<Reg> get_reg(const unsigned int &rid) const;
};

#endif // !HASHI_DIR

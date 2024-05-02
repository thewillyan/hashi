#ifndef HASHI_BUCKET
#define HASHI_BUCKET

#include <ios>
#include <string>

class Bucket {
private:
  unsigned short ld;
  unsigned int key;
  const std::string path;

public:
  Bucket(unsigned short local_deep, unsigned int key, std::string bucket_dir);

  // getters
  unsigned short get_local_deep() const;
  unsigned int get_key() const;
  const std::string &get_path() const;

  void incr_local_deep();

  std::fstream get_fstream(const std::ios::openmode &flags) const;
};

#endif // !HASHI_BUCKET

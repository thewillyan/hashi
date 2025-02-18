#ifndef HASHI_STREAM
#define HASHI_STREAM

#include "hashi/include/reg.hpp"
#include <istream>
#include <string>

Reg parseCsv(const std::string &s);
std::string regToCsv(const Reg &r);

class RegStream {
private:
  std::istream &input;

public:
  RegStream(std::istream &stream);
  ~RegStream();

  bool endOfStream() const;
  // skip `n` registers
  void skip(const size_t &n);

  // operators
  RegStream &operator>>(Reg &r);
};

#endif

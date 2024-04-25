#include "hashi/include/reg.hpp"
#include <istream>
#include <string>

Reg parseCsv(const std::string &s);

class RegStream {
private:
  std::istream &input;

public:
  RegStream(std::istream &stream);
  ~RegStream();

  // methods
  bool endOfStream() const;

  // operators
  RegStream &operator>>(Reg &r);
};

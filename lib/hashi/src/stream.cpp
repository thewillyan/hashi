#include "hashi/include/stream.hpp"
#include <istream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

Reg parseCsv(const std::string &s) {
  std::stringstream sstream(s);

  unsigned int id;
  double value;
  unsigned int year;

  unsigned int i = 0;

  while (sstream.good()) {
    std::string token;
    std::getline(sstream, token, ',');

    switch (i) {
    case 0:
      id = std::stoul(token);
      break;
    case 1:
      value = std::stod(token);
      break;
    case 2:
      year = std::stoul(token);
      break;
    default:
      throw std::runtime_error("Invalid CSV line, too much entries.");
      break;
    }
    ++i;
  }

  return Reg(id, value, year);
}

std::string regToCsv(const Reg &r) {
  std::ostringstream oss;
  oss << r.get_id() << ',' << r.get_value() << ',' << r.get_year();
  return oss.str();
}

RegStream::RegStream(std::istream &istream) : input{istream} {}
RegStream::~RegStream() {}

bool RegStream::endOfStream() const { return input.eof(); }

void RegStream::skip(const size_t &n) {
  for (size_t i = 0; i < n; ++i) {
    input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
}

// operators
RegStream &RegStream::operator>>(Reg &r) {
  std::string line;

  if (std::getline(input, line)) {
    r = Reg(parseCsv(line));
  }
  return *this;
}

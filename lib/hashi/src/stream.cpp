#include "hashi/include/stream.hpp"
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

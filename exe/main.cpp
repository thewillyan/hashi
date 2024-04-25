#include "hashi/include/stream.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

// NOTE: RegStream example
int main() {
  std::string file_path = "../db/table.csv";
  auto fs = std::ifstream(file_path);
  if (fs.fail()) {
    throw std::runtime_error(
        "Failed to open file stream, does the file exist?");
  }

  auto rs = RegStream(fs);
  Reg r;
  std::cout << "DATA on " << file_path << std::endl;
  while (!rs.endOfStream()) {
    rs >> r;
    std::cout << r << std::endl;
  }
}

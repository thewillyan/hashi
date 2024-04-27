#include "hashi/include/avail.hpp"
#include <bit>
#include <filesystem>
#include <fstream>
#include <stdexcept>

AvailFileParser::AvailFileParser(std::string file_path)
    : avail_file{file_path} {

  if (!std::filesystem::exists(avail_file)) {
    std::ofstream out_file(file_path, std::ios::binary);
    if (!out_file.is_open()) {
      throw std::runtime_error("Failed to create the availability file");
    };

    char buffer[2]{0, 0};
    out_file.write(buffer, 2);

    out_file.close();
    num_entries = 0;
  } else {
    std::ifstream in_file(avail_file, std::ios::binary);

    if (!in_file.is_open()) {
      throw std::runtime_error("Failed to open the availability file");
    };
    in_file.read(reinterpret_cast<char *>(&num_entries), 2);
    in_file.close();
  }
}

unsigned short AvailFileParser::get_num_entries() const { return num_entries; }

std::optional<size_t> AvailFileParser::get_avail_idx() const {
  std::ifstream file(avail_file, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the availability file");
  };

  file.seekg(2, std::ios::cur);

  size_t bit_pos;
  std::uint8_t b;
  while (!file.eof()) {
    file.read(reinterpret_cast<char *>(&b), 1);
    bit_pos = std::countl_zero(b);
    if (bit_pos < 8) {
      return bit_pos;
    }
  }
  file.close();

  if (bit_pos < 8) {
    return bit_pos;
  }
  return {};
}

std::vector<size_t> AvailFileParser::get_all_unavail() const {
  std::ifstream file(avail_file, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the availability file");
  };
  file.seekg(2, std::ios::cur);

  std::vector<size_t> unavail;
  std::uint8_t b;
  size_t bytes_readed = 0;
  size_t bits_readed = 0;
  std::uint8_t mask = 0b10000000;
  while (!file.eof()) {
    file.read(reinterpret_cast<char *>(&b), 1);

    for (size_t i = 0; i < 8; i++) {
      if ((b & (mask >> i)) == 0) {
        unavail.push_back(i + (bytes_readed * 8));
        if (++bits_readed == num_entries) {
          break;
        }
      }
    }
    ++bytes_readed;
  }
  file.close();

  return unavail;
}

void AvailFileParser::incr_num_entries() {
  std::ofstream file(avail_file, std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the availability file");
  };

  std::uint16_t new_num_entries = num_entries + 1;
  char buffer[2];

  buffer[0] = new_num_entries & 0xFF;
  buffer[1] = (new_num_entries >> 8) & 0xFF;

  file.write(buffer, 2);
  file.close();
  num_entries = new_num_entries;
}

void AvailFileParser::set_as_avail(size_t idx) const {
  // if (idx >= num_entries) {
  //   throw std::runtime_error("Invalid idx id on set_as_avail");
  // }

  std::fstream file(avail_file,
                    std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the availability file to read");
  };

  size_t skip = 2 + (idx / 8);
  file.seekg(skip, std::ios::beg);

  char byte = file.get();

  size_t byte_pos = idx % 8;
  std::uint8_t mask = 0b10000000 >> byte_pos;
  byte = byte | mask;

  file.seekp(skip, std::ios::beg);
  file.put(byte);
  file.close();
}

void AvailFileParser::set_as_unavail(size_t idx) const {
  // if (idx >= num_entries) {
  //   throw std::runtime_error("Invalid idx id on set_as_unavail");
  // }

  std::fstream file(avail_file,
                    std::ios::in | std::ios::out | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the availability file to read");
  };

  size_t skip = 2 + (idx / 8);
  file.seekg(skip, std::ios::beg);
  char byte = file.get();

  size_t byte_pos = idx % 8;
  std::uint8_t mask = 0b01111111 >> byte_pos;
  byte = byte & mask;

  file.seekp(skip, std::ios::beg);
  file.put(byte);
  file.close();
}

void AvailFileParser::add_entry() {
  if (num_entries % 8 == 0) {
    std::ofstream out_file(avail_file, std::ios::binary | std::ios::app);
    if (!out_file.is_open()) {
      throw std::runtime_error("Failed to open the availability file to write");
    };
    char byte = 0;
    out_file.write(&byte, 1);
    out_file.close();
  }
  incr_num_entries();
}

#include "hashi/include/db.hpp"
#include "hashi/include/stream.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

DbTable::DbTable(const std::string &db_path, const std::string &table_name)
    : table_fpath{db_path + "/" + table_name + ".csv"},
      avail{AvailFileParser(db_path + "/" + table_name + "_avail.bin")} {

  if (!std::filesystem::exists(table_fpath)) {
    std::ofstream table_file(table_fpath);
    if (!table_file.is_open()) {
      throw std::runtime_error("Failed to create table file");
    }
    table_file.close();
  }
}

void DbTable::add_reg(const Reg &r) {
  std::ofstream table_file(table_fpath, std::ios::app);
  if (!table_file.is_open()) {
    throw std::runtime_error("Failed to open table file");
  }

  table_file << regToCsv(r) << "\n";
  table_file.close();
  avail.add_entry();
}

void DbTable::del_reg(const size_t &page_number) {
  if (page_number > avail.get_num_entries() || page_number == 0) {
    throw std::runtime_error("Invalid register index, could not delete");
  }
  avail.set_as_avail(page_number - 1);
}

#include "hashi/include/db.hpp"
#include "hashi/include/stream.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

DbTable::DbTable(const std::string &db_path, const std::string &table_name)
    : table_fpath{db_path + "/" + table_name + ".csv"},
      avail{AvailFileParser(db_path + "/" + table_name + "_avail.bin")} {
  std::ofstream table_file(table_fpath);
  if (!table_file.is_open()) {
    throw std::runtime_error("Failed to create table file");
  }
  table_file.close();
}

void DbTable::add_reg(const Reg &r) {
  std::ofstream table_file(table_fpath, std::ios::app);

  if (!table_file.is_open()) {
    throw std::runtime_error("Failed to open table file");
  }

  table_file << regToCsv(r);
  table_file.close();
  avail.incr_num_entries();
}

void DbTable::del_reg(const size_t &reg_idx) { avail.set_as_avail(reg_idx); }

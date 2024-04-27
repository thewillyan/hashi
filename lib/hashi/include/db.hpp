#ifndef HASHI_DB
#define HASHI_DB

#include "hashi/include/avail.hpp"
#include "hashi/include/reg.hpp"

class DbTable {
private:
  const std::string table_fpath;
  AvailFileParser avail;

public:
  DbTable(const std::string &db_path, const std::string &table_name);

  void add_reg(const Reg &r);
  void del_reg(const size_t &reg_idx);
};

#endif

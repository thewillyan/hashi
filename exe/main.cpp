#include "hashi/include/db.hpp"

// NOTE: DbTable example
int main() {
  auto table = DbTable("../db", "tabela");
  table.add_reg(Reg(1, 53.2, 1998));
  table.add_reg(Reg(2, 1240.7, 2004));
  table.add_reg(Reg(3, 955.83, 1995));

  // delete the 2nd Reg (line 2 in the database file)
  table.del_reg(2);
}

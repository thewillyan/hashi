#include "hashi/include/dir.hpp"

// NOTE: Debug HashiDir
int main() {
  HashiDir dir{"../db", "setup"};

  dir.add_reg(Reg{1, 5801.17, 1998});
  dir.add_reg(Reg{2, 9958.7, 2024});
  dir.add_reg(Reg{3, 5594.44, 1995});
  dir.add_reg(Reg{4, 9863.24, 2012});
  dir.add_reg(Reg{5, 3364.52, 2013});
  dir.add_reg(Reg{6, 171.22, 1992});
  dir.add_reg(Reg{7, 6832.88, 2020});
  dir.add_reg(Reg{8, 5464.65, 2013});
  dir.add_reg(Reg{9, 7145.41, 2013});
  dir.add_reg(Reg{10, 8961.92, 2005});

  dir.add_reg(Reg{13, 15594.44, 1996});
  dir.add_reg(Reg{17, 16594.44, 1997});
  dir.add_reg(Reg{21, 17594.44, 1998});
  dir.add_reg(Reg{25, 18594.44, 1999});
  dir.add_reg(Reg{33, 45594.84, 2000});

  dir.del_reg(1);
  dir.del_reg(17);
  dir.del_reg(33);

  dir.add_reg(Reg{17, 16594.44, 1997});
  return 0;
}

#include "hashi/include/dir.hpp"
#include "ui/include/cli.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

// NOTE: CLI
int main() {
  std::ifstream in_file{"../in.txt"};
  if (!in_file.is_open()) {
    throw std::runtime_error("Could not find the in.txt file");
  }
  std::ofstream out_file{"../out.txt"};
  if (!in_file.is_open()) {
    throw std::runtime_error("Could not create the out.txt file");
  }

  std::string first_line;
  std::getline(in_file, first_line);
  std::string gd_str = first_line.substr(first_line.find('/') + 1);
  unsigned short gd = static_cast<unsigned short>(std::stoul(gd_str));

  out_file << first_line << std::endl;

  HashiDir dir{"../db", "compras", gd};

  std::vector<Cmd> cmds = parseCmds(in_file);
  in_file.close();

  for (const Cmd &cmd : cmds) {
    out_file << cmd.kind << ':' << cmd.value << '/';
    switch (cmd.kind) {
    case CmdKind::INC: {
      auto log = dir.add_reg_and_log(Reg{cmd.value, 0, 1900});
      out_file << log.gd << ',' << log.ld << std::endl;
      break;
    }
    case CmdKind::REM: {
      auto log = dir.del_reg_and_log(cmd.value);
      out_file << log.removed << ',' << log.gd << ',' << log.ld << std::endl;
      break;
    }
    case CmdKind::BUS: {
      auto log = dir.get_reg_and_log(cmd.value);
      out_file << log.selected << std::endl;
      break;
    }
    }
  }
  out_file << "P:/" << dir.get_global_deep();
  out_file.close();
  return 0;
}
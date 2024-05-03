#include "ui/include/cli.hpp"
#include <sstream>
#include <string>
#include <vector>

#include <iostream>

// TODO: parse "PG/<profundidade global inicial>"

// NOTE: CLI
int main() {

  std::string str{"INC:1\nREM:1\nBUS=:1"};
  std::stringstream ss{str};

  std::vector<Cmd> cmds = parseCmds(ss);

  for (const auto &cmd : cmds) {
    std::cout << cmd.kind << '(' << cmd.value << ')' << std::endl;
  }
  return 0;
}

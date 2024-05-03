#include "ui/include/cli.hpp"
#include <sstream>
#include <stdexcept>
#include <string>

std::ostream &operator<<(std::ostream &os, const CmdKind &kind) {
  switch (kind) {
  case CmdKind::INC:
    os << "INC";
    break;
  case CmdKind::REM:
    os << "REM";
    break;
  case CmdKind::BUS:
    os << "BUS";
    break;
  }
  return os;
}

CmdKind strToCmdKind(const std::string &str) {
  if (str == "INC") {
    return CmdKind::INC;
  } else if (str == "REM") {
    return CmdKind::REM;
  } else if (str == "BUS=") {
    return CmdKind::BUS;
  } else {
    throw std::runtime_error("Invalid CmdKind string '" + str + "'");
  }
}

// Função para analisar os comandos e valores
std::vector<Cmd> parseCmds(std::istream &input) {
  std::vector<Cmd> cmds;
  std::string line;
  std::string tmp;
  CmdKind k;
  unsigned int v;

  while (getline(input, line)) {
    std::stringstream ss{line};
    getline(ss, tmp, ':');
    k = strToCmdKind(tmp);
    getline(ss, tmp, ':');
    v = std::stoul(tmp);

    cmds.push_back(Cmd{k, v});
  }

  return cmds;
}

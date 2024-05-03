#ifndef HASHI_CLI
#define HASHI_CLI

#include <istream>
#include <vector>

enum class CmdKind {
  INC,
  REM,
  BUS,
};

std::ostream &operator<<(std::ostream &os, const CmdKind &kind);

CmdKind strToCmdKind(const std::string &str);

struct Cmd {
  CmdKind kind;
  unsigned int value;
};

std::vector<Cmd> parseCmds(std::istream &input);

#endif // !HASHI_CLI

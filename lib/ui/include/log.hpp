#ifndef HASHI_LOG
#define HASHI_LOG

struct IncLog {
  unsigned short gd;
  unsigned short ld;
};

struct RemLog {
  unsigned int removed;
  unsigned short gd;
  unsigned short ld;
};

struct BusLog {
  unsigned int selected;
};

#endif // !HASHI_LOG

#ifndef HASHI_REG
#define HASHI_REG

#include <ostream>

class Reg {
private:
  unsigned int id;
  double value;
  unsigned int year;

public:
  Reg(unsigned int id, double value, unsigned int year);
  Reg(Reg &&other);
  Reg();

  // getters
  unsigned int get_id() const;
  double get_value() const;
  unsigned int get_year() const;

  // setters
  void set_id(unsigned int);
  void set_value(double);
  void set_year(unsigned int);

  // operators
  Reg &operator=(Reg &&other) noexcept;
  bool operator==(const Reg &other) const;
  friend std::ostream &operator<<(std::ostream &os, const Reg &r);
};

#endif

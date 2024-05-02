#include "hashi/include/reg.hpp"

Reg::Reg(unsigned int i, double v, unsigned int y) : id{i}, value{v}, year{y} {}
Reg::Reg() : id{0}, value{0.0}, year{0} {}
Reg::Reg(Reg &&other) : id{other.id}, value{other.value}, year{other.year} {}

// getters
unsigned int Reg::get_id() const { return id; }
double Reg::get_value() const { return value; }
unsigned int Reg::get_year() const { return year; }

// setters
void Reg::set_id(unsigned int i) { id = i; }
void Reg::set_value(double v) { value = v; }
void Reg::set_year(unsigned int y) { year = y; }

// operators
Reg &Reg::operator=(Reg &&other) noexcept {
  id = other.id;
  value = other.value;
  year = other.year;
  return *this;
}

bool Reg::operator==(const Reg &other) const {
  return (id == other.id) && (value == other.value) && (year == other.year);
}

std::ostream &operator<<(std::ostream &os, const Reg &r) {
  os << "Reg { id: " << r.id << ", value: " << r.value << ", year: " << r.year
     << " }";
  return os;
}

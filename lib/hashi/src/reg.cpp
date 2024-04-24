#include "hashi/include/reg.hpp"

Reg::Reg(unsigned int i, double v, unsigned int y) : id{i}, value{v}, year{y} {}

// getters
unsigned int Reg::get_id() const { return id; }
double Reg::get_value() const { return value; }
unsigned int Reg::get_year() const { return year; }

// setters
void Reg::set_id(unsigned int i) { id = i; }
void Reg::set_value(double v) { value = v; }
void Reg::set_year(unsigned int y) { year = y; }

// operators
bool Reg::operator==(const Reg &other) const {
  return (id == other.id) && (value == other.value) && (year == other.year);
}

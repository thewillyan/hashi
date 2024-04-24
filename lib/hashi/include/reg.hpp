class Reg {
private:
  unsigned int id;
  double value;
  unsigned int year;

public:
  Reg(unsigned int id, double value, unsigned int year);

  // getters
  unsigned int get_id() const;
  double get_value() const;
  unsigned int get_year() const;

  // setters
  void set_id(unsigned int);
  void set_value(double);
  void set_year(unsigned int);

  // operators
  bool operator==(const Reg &other) const;
};

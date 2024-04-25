class Reg {
private:
  unsigned int id;
  double value;
  unsigned int year;

public:
  Reg(unsigned int id, double value, unsigned int year);
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
  Reg &operator=(Reg &other) = delete;
  bool operator==(const Reg &other) const;
};

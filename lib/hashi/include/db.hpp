#include <vector>

// NOTE: this is a dummy class for purposes of setup
class Database {
private:
  std::vector<int> data;

public:
  Database();
  void add_data(int item);
  int get_last() const;
};

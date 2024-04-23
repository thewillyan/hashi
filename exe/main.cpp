#include "hashi/include/db.hpp"
#include <iostream>

// NOTE: Dummy binary
int main() {
  auto db = Database();
  db.add_data(5);
  db.add_data(20);
  std::cout << "The last item added was: " << db.get_last() << std::endl;
}

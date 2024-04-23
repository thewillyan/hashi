#include "hashi/include/db.hpp"
#include <vector>

// NOTE: Dummy implementations
Database::Database() : data{std::vector<int>()} {}
void Database::add_data(int item) { data.push_back(item); }
int Database::get_last() const { return data.back(); }

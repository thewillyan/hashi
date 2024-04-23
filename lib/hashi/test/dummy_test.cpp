#include "catch2/catch_test_macros.hpp"
#include "hashi/include/db.hpp"

TEST_CASE("Dummy database works", "[dummy]") {
  auto db = Database();
  db.add_data(5);
  db.add_data(20);
  REQUIRE(db.get_last() == 20);
}

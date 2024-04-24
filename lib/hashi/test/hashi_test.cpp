#include "catch2/catch_test_macros.hpp"
#include "hashi/include/stream.hpp"
#include <cstddef>

TEST_CASE("CSV parser works", "[csvParser]") {
  std::string csv_strs[10]{"1,5801.17,1998", "2,9958.7,2024",  "3,5594.44,1995",
                           "4,9863.24,2012", "5,3364.52,2013", "6,171.22,1992",
                           "7,6832.88,2020", "8,5464.65,2013", "9,7145.41,2013",
                           "10,8961.92,2005"};

  Reg answers[10]{Reg(1, 5801.17, 1998), Reg(2, 9958.7, 2024),
                  Reg(3, 5594.44, 1995), Reg(4, 9863.24, 2012),
                  Reg(5, 3364.52, 2013), Reg(6, 171.22, 1992),
                  Reg(7, 6832.88, 2020), Reg(8, 5464.65, 2013),
                  Reg(9, 7145.41, 2013), Reg(10, 8961.92, 2005)};

  for (size_t i = 0; i < 10; ++i) {
    REQUIRE(parseCsv(csv_strs[i]) == answers[i]);
  }
}

#include "catch2/catch_test_macros.hpp"
#include "hashi/include/stream.hpp"
#include <cstddef>
#include <sstream>

std::string CSV{"1,5801.17,1998\n"
                "2,9958.7,2024\n"
                "3,5594.44,1995\n"
                "4,9863.24,2012\n"
                "5,3364.52,2013\n"
                "6,171.22,1992\n"
                "7,6832.88,2020\n"
                "8,5464.65,2013\n"
                "9,7145.41,2013\n"
                "10,8961.92,2005"};

const std::string CSV_STRS[10]{
    "1,5801.17,1998", "2,9958.7,2024",  "3,5594.44,1995", "4,9863.24,2012",
    "5,3364.52,2013", "6,171.22,1992",  "7,6832.88,2020", "8,5464.65,2013",
    "9,7145.41,2013", "10,8961.92,2005"};

const Reg ANSWERS[10]{Reg(1, 5801.17, 1998), Reg(2, 9958.7, 2024),
                      Reg(3, 5594.44, 1995), Reg(4, 9863.24, 2012),
                      Reg(5, 3364.52, 2013), Reg(6, 171.22, 1992),
                      Reg(7, 6832.88, 2020), Reg(8, 5464.65, 2013),
                      Reg(9, 7145.41, 2013), Reg(10, 8961.92, 2005)};

TEST_CASE("CSV parser works", "[csvParser]") {
  for (size_t i = 0; i < 10; ++i) {
    REQUIRE(parseCsv(CSV_STRS[i]) == ANSWERS[i]);
  }
}

TEST_CASE("Register stream works", "[RegStream]") {
  auto ss = std::stringstream(CSV);
  auto stream = RegStream(ss);
  Reg r;
  for (size_t i = 0; i < 10; ++i) {
    stream >> r;
    REQUIRE(r == ANSWERS[i]);
  }
  REQUIRE(stream.endOfStream());
}

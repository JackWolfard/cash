#include "common.h"

TEST_CASE("basics", "[basics]") {
  SECTION("assign", "[assign]") {
    TEST([]()->ch_bit1 {
      ch_bit4 a, b, c;
      a = 0xf_h;
      b = a;
      c = b + 1;
      b = 0;
      return (c == 1);
    });

    TEST([]()->ch_bit1 {
      ch_bit2 a, b;
      a = 0;
      b = a;
      a = 1;
      return (b == 1);
    });
  }
}

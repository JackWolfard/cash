#include "common.h"

TEST_CASE("literals", "[literals]") {
  SECTION("binary", "[binary]") {
    TEST([]()->ch_bool {
      ch_bit4 a(0x5);
      return (a == 0101_b);
    });
    TEST([]()->ch_bool {
      ch_bit4 a(0x5);
      return (a == ch_sbit<4>("0101_b"));
    });
    TEST([]()->ch_bool {
      ch_bit4 a(0x5);
      return (a == 00000000101_b4);
    });
    TEST([]()->ch_bool {
      ch_bit<3> a(0x7);
      return (a == 0x7_h3);
    });
  }
  SECTION("octal", "[octal]") {
    TEST([]()->ch_bool {
      ch_bit<3> a(0x5);
      return (a == 5_o);
    });
    TEST([]()->ch_bool {
      ch_bit<3> a(0x5);
      return (a == ch_sbit<3>("5_o"));
    });
    TEST([]()->ch_bool {
      ch_bit64 x(0x1c0000000_h64);
      return (x == 070000000000_o64);
    });
  }
  SECTION("hexadecimal", "[hexadecimal]") {
    TEST([]()->ch_bool {
      ch_bit4 a(0x5);
      return (a == 0x5_h);
    });
    TEST([]()->ch_bool {
      ch_bit4 a(0x5);
      return (a == ch_sbit<4>("5_h"));
    });
    TEST([]()->ch_bool {
      ch_bit4 a(0x5);
      return (a == 0x5_h);
    });
    TEST([]()->ch_bool {
      ch_bit64 a(0x5);
      return (a == 0x5_h64);
    });
    TEST([]()->ch_bool {
      ch_bit64 a(0x5);
      return (a == 0x00'05_h64);
    });
    TEST([]()->ch_bool {
      ch_bit8 a(0x1'0_h);
      return (a == 16);
    });
    TEST([]()->ch_bool {
      ch_bit32 a(0x0001'0000_h);
      return (ch_slice<4>(a, 4*4) == 0x1_h);
    });
    TEST([]()->ch_bool {
      ch_bit32 a(0x00'01'00'00_h32);
      return (ch_slice<4>(a, 4*4) == 0x1_h);
    });
    TEST([]()->ch_bool {
      const ch_bit32 a(0x00'01'00'00_h32);
      return (ch_slice<4>(a, 4*4) == 0x1_h);
    });
    TEST([]()->ch_bool {
      ch_bit64 a(0x10000000'00000000_h);
      return (ch_slice<4>(a, 4*15) == 0x1_h);
    });
  }
}

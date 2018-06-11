#include "common.h"

__enum (e2_t, 2, (
  (idle, 0),
  execute,
  stats,
  done
));

__union (u4_t, (
  (ch_bit4) a
  ));

__union (u2_4_t, (
  (ch_bit2) a,
  (ch_bit4) b
  ));

__struct (s4_t, (
  (ch_bit4) a
  ));

__struct (s2_4_t, (
  (ch_bit2) a,
  (ch_bit4) b
));

__union (ux_t, (
  (u2_4_t) a,
  (s2_4_t) b
));

__struct (sd1_t, s4_t, (
  (ch_bit4) b
));

__struct (sd2_t, sd1_t, (
  (ch_bit4) c
));

__struct (sd3_t, (
  (sd1_t)   c,
  (ch_bit4) d
));

using v2_1_t = ch_vec<ch_bit2, 1>;
using v2_2_t = ch_vec<ch_bit2, 2>;
using v2_3_t = ch_vec<ch_bit2, 3>;

TEST_CASE("scalars", "[scalars]") {
  SECTION("basics", "[basics]") {
    TESTX([]()->bool {
      ch_scuint<4> a(0xa);
      ch_scuint<4> b(0x5);
      int v(a + b);
      int ret = v + 1;
      return (ret == 0x10);
    });
    TESTX([]()->bool {
      ch_scuint<4> a(0), b(1), c(1);
      int ret = (a != b);
      ret &= (b == c);
      ret &= (a == (b - c));
      return !!ret;
    });
    TESTX([]()->bool {
      ch_scuint<4> a(0), b(1), c(1);
      int ret = (a != b);
      ret &= (b == c);
      ret &= (a == (b - c));
      return !!ret;
    });
    TESTX([]()->bool {
      ch_scalar<4> a(0101_b);
      return (a[0] == a[2]) && (a.slice<2>() == a.slice<2>(2));
    });
    TESTX([]()->bool {
      const ch_scalar<4> a(0101_b);
      return (a[0] == a[2]) && (a.slice<2>() == a.slice<2>(2));
    });
    TESTX([]()->bool {
      auto force_move_assignment = []() {
        return 4_h + 3_h;
      };
      ch_scalar<4> x;
      x = force_move_assignment();
      return (x == 7);
    });
    TESTX([]()->bool {
      ch_scuint<4> a(e2_t::done), b(1);

      int ret = (a == 3);
      ret &= ((a + b) == 4);
      ret &= ((a - 1) == 2);
      ret &= ((a & b) == 1);
      ret &= ((a | 1) == 3);
      ret &= (~a == 1100_b);
      ret &= (~b == 1110_b);
      ret &= ((a >> 1) == 1);
      ret &= ((b << 1) == 2); //?
      ret &= ((1 >> a) == 0);
      ret &= ((1 << b) == 2);
      return !!ret;
    });
  }
  SECTION("cast", "[cast]") {
    TESTX([]()->bool {
      ch_scalar<4> x(8);
      return (x == 8);
    });
    TESTX([]()->bool {
      ch_scuint<4> x(8);
      return (x == 8);
    });
    TESTX([]()->bool {
      ch_scint<4> x(8);
      return (x == 8);
    });
    TESTX([]()->bool {
      ch_scalar<4> x(8);
      return (static_cast<int>(x) == 8);
    });
    TESTX([]()->bool {
      ch_scuint<4> x(8);
      return (static_cast<int>(x) == 8);
    });
    TESTX([]()->bool {
      ch_scint<4> x(8);
      return (static_cast<int>(x) == -8);
    });
    TESTX([]()->bool {
      ch_scint<4> x(7);
      return (static_cast<int>(x) == 7);
    });
    TESTX([]()->bool {
      ch_scalar<4> x(-8);
      return (x == 8);
    });
    TESTX([]()->bool {
      ch_scuint<4> x(-8);
      return (x == 8);
    });
    TESTX([]()->bool {
      ch_scint<4> x(-8);
      return (x == -8);
    });
    TESTX([]()->bool {
      ch_scalar<4> x(-8);
      return (static_cast<int>(x) == 8);
    });
    TESTX([]()->bool {
      ch_scuint<4> x(-8);
      return (static_cast<int>(x) == 8);
    });
    TESTX([]()->bool {
      ch_scint<4> x(-8);
      return (static_cast<int>(x) == -8);
    });
    TESTX([]()->bool {
      ch_scalar<4> x(-1);
      return (static_cast<int>(x) == 0xf);
    });
    TESTX([]()->bool {
      ch_scuint<4> x(-1);
      return (static_cast<int>(x) == 0xf);
    });
    TESTX([]()->bool {
      ch_scint<4> x(-1);
      return (static_cast<int>(x) == -1);
    });
  }
  SECTION("enum", "[enum]") {
    TESTX([]()->bool {
      ch_scalar_t<e2_t> a(e2_t::done);
      return (a == 3);
    });
    TESTX([]()->bool {
      ch_scalar_t<e2_t> a(e2_t::done), b(a);
      return (a == b);
    });
  }
  SECTION("union", "[union]") {
    TESTX([]()->bool {
      ch_scalar_t<u2_4_t> u2(0101_b);
      int ret = (u2.a == 1);
      ret &= (u2.b == 5);
      u2.b = 7;
      ret &= (u2.a == 3);
      u2.a = 0;
      ret &= (u2.b == 4);
      return !!ret;
    });
    TESTX([]()->bool {
      ch_scalar_t<u2_4_t> a(0101_b), b(a);
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<u2_4_t> a(0101_b), b;
      b = a;
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<u2_4_t> a(3);
      return (a.as_scalar() == 3);
    });
    TESTX([]()->bool {
      ch_scalar_t<u2_4_t> a(0011_b4);
      return (a.as_scalar() == 3);
    });
    TESTX([]()->bool {
      ch_scalar_t<u2_4_t> a(11_b2);
      return (a.as_scalar() == 3);
    });
  }
  SECTION("struct", "[struct]") {
    TESTX([]()->bool {
      ch_scalar_t<s2_4_t> s2(0101_b, 01_b);
      int ret = (s2.a == 1);
      ret &= (s2.b == 5);
      s2.b = 7;
      ret &= (s2.a == 1);
      s2.a = 0;
      ret &= (s2.b == 7);
      return !!ret;
    });
    TESTX([]()->bool {
      auto force_move_assignment = []() {
        return 4_h + 3_h;
      };
      ch_scalar_t<s2_4_t> s2(0101_b, 01_b);
      s2.b = force_move_assignment();
      return (s2.b == 7) && (s2.as_scalar() == 011101_b);
    });
    TESTX([]()->bool {
      ch_scalar_t<s2_4_t> a(010101_b), b(0101_b, 01_b);
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<s2_4_t> a(010101_b), b(a);
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<s2_4_t> a(010101_b), b;
      b = a;
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<sd1_t> a(21_h), b(2_h, 1_h);
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<sd3_t> a(321_h), b(3_h, 21_h);
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<sd3_t> a(321_h), b(3_h, 21_h);
      return (a.as_scalar() == b.as_scalar());
    });
    TESTX([]()->bool {
      ch_scalar_t<sd3_t> a(321_h), b(a);
      return (b.d == 3);
    });
    TESTX([]()->bool {
      auto force_move_assignment = []() {
        return ch_scalar_t<sd3_t>{321_h};
      };
      ch_scalar_t<sd3_t> a;
      a = force_move_assignment();
      return (a.as_scalar() == 321_h);
    });
    TESTX([]()->bool {
      auto force_move_assignment = []() {
        return ch_scalar_t<sd3_t>{321_h};
      };
      ch_scalar_t<sd3_t> a(force_move_assignment());
      return (a.as_scalar() == 321_h);
    });
    TESTX([]()->bool {
      ch_scalar_t<sd3_t> s3(321_h);
      ch_scalar_t<sd1_t> x(54_h);

      int ret = (s3.c.a == 1);
      ret &= (s3.c.b == 2);
      ret &= (s3.d == 3);
      s3.c = x;
      ret &= (s3.c.a == 4);
      ret &= (s3.c.b == 5);
      ret &= (s3.d == 3);
      return !!ret;
    });
  }
  SECTION("vector", "[vector]") {
    TEST([]()->ch_bool {
      ch_scalar_t<v2_2_t> x(3_h);
      v2_2_t y(x.as_scalar());
      return (y[0] == 3) && (y[1] == 0);
    });
    TESTX([]()->bool {
      ch_scalar_t<v2_2_t> x(3_h);
      return (x[0] == 3) && (x[1] == 0);
    });
  }
}
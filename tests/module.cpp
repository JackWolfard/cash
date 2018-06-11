#include <htl/queue.h>
#include "common.h"

using namespace ch::htl;

static_assert(ch_direction_v<ch_in<ch_bool>> == ch_direction::in, "invalid direction");
static_assert(ch_direction_v<ch_out<ch_bool>> == ch_direction::out, "invalid direction");

__inout(io_bundle2_io, (
  __in(ch_bool) x,
  __in(ch_bool) y
));
static_assert(ch_direction_v<io_bundle2_io> == ch_direction::in, "invalid direction");

__inout(io_bundle1_io, (
  __out(ch_bool) x,
  __out(ch_bool) y
));
static_assert(ch_direction_v<io_bundle1_io> == ch_direction::out, "invalid direction");

__inout(io_bundle3_io, (
  __in(ch_bool) x,
  __out(ch_bool) y
));
static_assert(ch_direction_v<io_bundle3_io> == ch_direction::inout, "invalid direction");

__enum (e2_t, 2, (
  a, b, c, d
));

__struct (s4_2_t, (
  (ch_bit4) a,
  (ch_bit2) b
));

__struct (u4_2_t, (
  (ch_bit4) a,
  (ch_bit2) b
));

__inout(bundle4_io, (
  __in(e2_t) x,
  __in(s4_2_t) y,
  __out(u4_2_t) z,
  __out(ch_vec<ch_bool, 2>) w,
  (ch_enq_io<s4_2_t>) eq,
  (ch_deq_io<s4_2_t>) dq
));

template <typename T>
__inout(link_io, (
  __out(T) data,
  __out(ch_bool) valid
));

template <typename T>
__inout(plink_io, link_io<T>, (
  __out(ch_bool) parity
));

template <typename T>
__inout(filter_io, (
  (ch_flip_t<plink_io<T>>) x,
  (plink_io<T>) y
));

template <typename T>
struct Filter {
  filter_io<T> io;
  void describe() {
    auto tmp = (ch_pad<ch_width_v<T>+1>(io.x.data) << 1)
              | ch_pad<ch_width_v<T>+1>(io.x.parity);
    io.y.data   = ch_delay(ch_slice<T>(tmp));
    io.y.parity = tmp[ch_width_v<T>];
    io.y.valid  = ch_delay(io.x.valid);
  }
};

template <typename T>
struct FilterBlock {
  filter_io<T> io;
  void describe() {
    f1_.io.x(io.x);
    f1_.io.y(f2_.io.x);
    f2_.io.y(io.y);
  }
  ch_module<Filter<T>> f1_, f2_;
};

template <typename T, unsigned N>
struct QueueWrapper {
  __io (
    (ch_deq_io<T>) enq,
    (ch_enq_io<T>) deq
  );
  void describe() {
    queue_.io.enq(io.enq);
    queue_.io.deq(io.deq);
  }
  ch_module<ch_queue<T, N>> queue_;
};

struct Adder {
  __io (
    __in(ch_uint2)  in1,
    __in(ch_uint2)  in2,
    __out(ch_uint2) out
  );
  void describe() {
    io.out = io.in1 + io.in2;
  }
};

struct Foo1 {
  __io (
    __in(ch_bit2)  in1,
    __in(ch_bit2)  in2,
    __out(ch_bit2) out
  );
  void describe() {
    adder_.io.in1(io.in1);
    adder_.io.in2(io.in2);
    adder_.io.out(io.out);
  }
  ch_module<Adder> adder_;
};

struct Foo2 {
  ch_out<ch_bool> io;
  void describe() {
    ch_module<Adder> adder;
    adder.io.in1 = 1;
    adder.io.in2 = 2;
    io = (3 == adder.io.out);
  }
};

struct Foo3 {
  __inout(io_ab_t, (
    __in(ch_uint2) a,
    __out(ch_uint2) b
  ));

  __io(
    (ch_vec<io_ab_t, 2>) x,
    (ch_vec<ch_in<ch_uint2>, 2>) y,
    (ch_vec<ch_out<ch_uint2>, 2>) z
  );

  void describe() {
    for (int i = 0; i < 2; ++i) {
      io.x[i].b = io.y[i] + io.x[i].a;
      io.z[i] = io.x[i].b;
    }
  }
};

struct Loop {
  __io (
    __in(ch_uint4)  in1,
    __in(ch_uint4)  in2,
    __out(ch_uint4) out
  );

  void describe() {
    m1_.io.in1(io.in1);
    m1_.io.in2(io.in2);
    m2_.io.in(m1_.io.out1);
    m1_.io.in3(m2_.io.out);
    io.out(m1_.io.out2);
  }

  struct M1 {
    __io (
      __in(ch_uint4)  in1,
      __in(ch_uint4)  in2,
      __in(ch_uint4)  in3,
      __out(ch_uint4) out1,
      __out(ch_uint4) out2
    );

    void describe() {
      io.out1 = io.in1 + io.in2;
      io.out2 = -io.in3;
    }
  };

  struct M2 {
    __io (
      __in(ch_uint4)  in,
      __out(ch_uint4) out
    );

    void describe() {
      io.out = -io.in;
    }
  };

  ch_module<M1> m1_;
  ch_module<M2> m2_;
};

struct MultiClk {
  __io (
    __in(ch_uint4) in,
    __out(ch_uint4) out
  );

  void describe() {
    ch_reg<ch_uint4> x(0);

    ch_pushcd(x[0]);
    ch_reg<ch_uint4> y(0);
    ch_popcd();

    x <<= x + 1;
    y <<= io.in;
    io.out = x + y;

    //ch_print("{0}: clk={1}, rst={2}, in={3}, x={4}, y={5}, out={6}", ch_time(), ch_clock(), ch_reset(), io.in, x, y, io.out);
  }
};

struct CustomClk {
  __io (
    __in(ch_uint4) in,
    __out(ch_uint4) out
  );

  void describe() {
    ch_pushcd(~ch_clock());
    ch_reg<ch_uint4> x(io.in);
    ch_popcd();

    x <<= x + 1;
    io.out = x;

    //ch_print("{0}: clk={1}, rst={2}, in={3}, out={4}", ch_time(), ch_clock(), ch_reset(), io.in, io.out);
  }
};

TEST_CASE("module", "[module]") {
  SECTION("sim", "[sim]") {
    TESTX([]()->bool {
      ch_device<Adder> adder;
      adder.io.in1 = 1;
      adder.io.in2 = 2;
      ch_simulator sim(adder);
      sim.run(1);
      return (3 == adder.io.out);
    });

    TESTX([]()->bool {
      ch_device<Foo3> foo;
      ch_simulator sim(foo);
      for (int i = 0; i < 2; ++i) {
        foo.io.y[i] = i;
        foo.io.x[i].a = 2-i;
      }
      sim.run(1);
      int ret(1);
      for (int i = 0; i < 2; ++i) {
        ret &= (2 == foo.io.z[i]);
        ret &= (2 == foo.io.x[i].b);
      }
      return !!ret;
    });
  }
  SECTION("emplace", "[emplace]") {
    TESTX([]()->bool {
      ch_device<Foo1> foo;
      foo.io.in1 = 1;
      foo.io.in2 = 2;
      ch_simulator sim(foo);
      sim.run(1);
      return (3 == foo.io.out);
    });

    TESTX([]()->bool {
      ch_device<Foo2> foo;
      ch_simulator sim(foo);
      sim.run(1);
      return (bool)foo.io;
    });

    TESTX([]()->bool {
      ch_device<FilterBlock<ch_uint16>> filter;
      ch_simulator sim(filter);
      ch_tick t = sim.reset(0);

      filter.io.x.data   = 3;
      filter.io.x.valid  = 1;
      filter.io.x.parity = 0;
      t = sim.step(t, 2);

      int ret(!!filter.io.y.valid);
      ret &= (12 == filter.io.y.data);
      ret &= !filter.io.y.parity;

      ch_toVerilog("filter.v", filter);
      ret &= (checkVerilog("filter_tb.v"));
      ch_toFIRRTL("filter.fir", filter);

      return !!ret;
    });

    TESTX([]()->bool {
      ch_device<QueueWrapper<ch_bit4, 2>> queue;
      ch_simulator sim(queue);
      ch_tick t = sim.reset(0);

      int ret(!!queue.io.enq.ready);  // !full
      ret &= !queue.io.deq.valid; // empty
      queue.io.deq.ready = 0;
      queue.io.enq.data = 0xA;
      queue.io.enq.valid = 1; // push
      t = sim.step(t);

      ret &= !!queue.io.deq.valid;  // !empty
      queue.io.enq.data = 0xB;
      t = sim.step(t);

      ret &= !queue.io.enq.ready; // full
      ret &= !!queue.io.deq.valid;
      ret &= (0xA == queue.io.deq.data);
      queue.io.enq.valid = 0; // !push
      queue.io.deq.ready = 1; // pop
      t = sim.step(t);

      ret &= !!queue.io.enq.ready;  // !full
      ret &= (0xB == queue.io.deq.data);
      queue.io.deq.ready = 1; // pop
      t = sim.step(t, 4);

      ret &= !queue.io.deq.valid; // empty
      ch_toVerilog("queue.v", queue);
      ret &= (checkVerilog("queue_tb.v"));
      ch_toFIRRTL("queue.fir", queue);

      return !!ret;
    });

    TESTX([]()->bool {
      ch_device<MultiClk> device;
      ch_toVerilog("multi_clk.v", device);

      ch_simulator sim(device);
      device.io.in = 0xA;
      sim.run(10);
      std::cout << "out = "  << device.io.out << std::endl;
      ch_vcdtracer tracer("multi_clk.vcd", device);
      int ret = (device.io.out == 0xe);

      return !!ret;
    });

    TESTX([]()->bool {
      ch_device<CustomClk> device;
      ch_toVerilog("custom_clk.v", device);

      ch_simulator sim(device);
      device.io.in = 0xA;
      sim.run(10);
      std::cout << "out = "  << device.io.out << std::endl;
      ch_vcdtracer tracer("custom_clk.vcd", device);
      int ret = (device.io.out == 0xe);

      return !!ret;
    });

    TESTX([]()->bool {
      ch_device<Loop> loop;
      ch_toVerilog("loop.v", loop);
      ch_toFIRRTL("loop.fir", loop);

      loop.io.in1 = 1;
      loop.io.in2 = 2;
      ch_vcdtracer tracer("loop.vcd", loop);
      tracer.run();

      std::cout << "out = "  << loop.io.out << std::endl;
      int ret = (loop.io.out == 3);

      return !!ret;
    });
  }
}
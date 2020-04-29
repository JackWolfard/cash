#include <cash/core.h>
#include <cash/htl/elastic.h>

#include "common.h"

using namespace ch::core;
using namespace ch::htl;

// N is width of adder
// C is cycles needed to calculate for simulating delays
template <unsigned N, unsigned C>
struct ElasticAdder {
  __io(
    (ch_elastic<ch_uint<1>>)  cin,
    (ch_elastic<ch_uint<N>>)  lhs,
    (ch_elastic<ch_uint<N>>)  rhs,
    (ch_flip_io<ch_elastic<ch_uint<N>>>)  out,
    (ch_flip_io<ch_elastic<ch_uint<1>>>)  cout
  );

  void describe() {
    // elastic ops
    // all valids for ins should be together
    // all readies from outs should be together
    
    ch_elastic<ch_bool> internal_in;
    ch_flip_io<ch_elastic<ch_bool>> internal_out;

    internal_in.join<ch_uint<1>, ch_uint<N>>(io.cin, io.lhs);
    internal_in.join<ch_uint<1>, ch_uint<N>>(io.cin, io.rhs);
    internal_out.fork<ch_uint<N>, ch_uint<1>>(io.out, io.cout);

    // idk if this is needed
    internal_in.ready = true;
    internal_out.valid = true;
    
    __if (internal_in.valid && internal_out.ready) {
      auto sum = ch_pad<1>(io.lhs.data) + io.rhs.data + io.cin.data;
      io.out.data = ch_slice<N>(sum);
      io.out.valid = true;
      io.cout.data = sum[N];
      io.cout.valid = true;
    } __else {
      io.out.valid = false;
      io.cout.valid = true;
    };
  }
};

int main() {
  ch_device<ElasticAdder<2, 0>> adder;

  adder.io.cin.data = 1;
  adder.io.lhs.data = 1;
  adder.io.rhs.data = 3;

  // elastic ops
  adder.io.cin.valid = true;
  adder.io.lhs.valid = true;
  adder.io.rhs.valid = true;
  adder.io.out.ready = true;
  adder.io.cout.ready = true;

  ch_tracer tracer(adder);
  tracer.run();

  std::cout << "result:" << std::endl;
  std::cout << "cout = " << adder.io.cout.data << std::endl;
  std::cout << "out = "  << adder.io.out.data << std::endl;

  CHECK(adder.io.out.data == 1);
  CHECK(adder.io.cout.data == 1);
  return 0;
}


#pragma once

#include "reg.h"

namespace ch {
namespace internal {

template <typename T>
class ch_seq final : public const_type_t<T> {
public:
  using traits = logic_traits<bitwidth_v<T>, ch_seq, const_type_t<T>, T, scalar_type_t<T>>;
  using base = const_type_t<T>;

  T next;

  ch_seq() {
    auto reg = createRegNode(get_lnode<T>(next), get_lnode<int, bitwidth_v<T>>(0));
    bit_accessor::set_data(*this, reg);
    next = *this;
  }

  template <typename U,
            CH_REQUIRES(is_cast_convertible<T, U>::value)>
  explicit ch_seq(const U& init) {
    auto reg = createRegNode(get_lnode<T>(next), get_lnode<U, bitwidth_v<T>>(init));
    bit_accessor::set_data(*this, reg);
    next = *this;
  }

protected:

  ch_seq(ch_seq&) = delete;
  ch_seq(ch_seq&&) = delete;
  ch_seq& operator=(const ch_seq&) = delete;
  ch_seq& operator=(ch_seq&&) = delete;
};

}
}

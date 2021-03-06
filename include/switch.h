#pragma once

#include "if.h"

namespace ch {
namespace internal {

class switch_t {
public:

  switch_t(const lnode& key, const source_location& sloc) {
    begin_branch(key.impl(), sloc);
  }

  ~switch_t() {
    end_branch();
  }

  template <typename K> friend class switch_body_t;
};

using switch_ptr = std::shared_ptr<switch_t>;

template <typename K> class switch_case_t;

template <typename K>
class switch_body_t {
public:

  switch_body_t(const switch_ptr& p_switch, const sdata_type& value)
    : switch_(p_switch)
    , value_(value)
  {}

  switch_case_t<K> operator,(const fvoid_t& body);

protected:

  switch_ptr switch_;
  sdata_type value_;
};

template <typename K>
class switch_case_t {
public:

  switch_case_t(const switch_ptr& p_switch) : switch_(p_switch) {}

  template <typename P,
            CH_REQUIRES(!std::is_convertible_v<P, fvoid_t>)>
  switch_body_t<K> operator,(const P& pred) {
    static_assert(is_equality_comparable_v<K, P>, "invalid type");
    static_assert(!is_logic_type_v<P>, "invalid type");
    if constexpr (std::is_integral_v<P>) {
      return switch_body_t<K>(switch_, sdata_type(ch_width_v<K>, pred));
    } if constexpr (std::is_enum_v<P>) {
      return switch_body_t<K>(switch_, sdata_type(ch_width_v<K>, static_cast<int>(pred)));
    } else {
      return switch_body_t<K>(switch_, static_cast<sdata_type>(pred));
    }
  }

  void operator,(const fvoid_t& body) {
    cond_block(body);
  }

protected:

  switch_ptr switch_;
};

template <typename K>
switch_case_t<K> switch_body_t<K>::operator,(const fvoid_t& body) {
  cond_block(value_, body);
  return switch_case_t<K>(switch_);
}

template <typename K>
auto ch_switch(const K& key, CH_SLOC) {
  static_assert(is_bitbase_v<K>, "invalid type");
  return switch_case_t<K>(std::make_shared<switch_t>(get_lnode(key), sloc));
}

}
}

#define CH_SWITCH(key)  ch_switch(key)
#define CH_CASE(pred)   , pred, [&]()
#define CH_DEFAULT      , [&]()

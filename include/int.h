#pragma once

#include "numberbase.h"

namespace ch {
namespace internal {

template <unsigned N = 32>
class ch_sint : public ch_snumber_base<ch_sint<N>> {
public:
  using traits = system_traits<N, true, ch_sint, ch_int<N>>;
  template <unsigned M> using size_cast = ch_sint<M>;
  using base = ch_snumber_base<ch_sint<N>>;
  using base::operator=;

  ch_sint(const system_buffer_ptr& buffer
          = make_system_buffer(N, idname<ch_sint>()))
    : buffer_(buffer) {
    assert(N == buffer->size());
  }

  template <typename U,
            CH_REQUIRE(std::is_integral_v<U>)>
  ch_sint(const U& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    base::operator=(other);
  }

  template <typename U,
            CH_REQUIRE(is_bitvector_extended_type_v<U>)>
  explicit ch_sint(U&& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    this->operator=(make_system_buffer(sdata_type(N , std::forward<U>(other))));
  }

  template <typename U,
            CH_REQUIRE(ch_width_v<U> <= N)>
  explicit ch_sint(const ch_sbit_base<U>& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    base::operator=((const U&)other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_sint(const ch_sbit<M>& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    base::operator=(other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_sint(const ch_suint<M>& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    base::operator=(other);
  }

  template <unsigned M,
            CH_REQUIRE(M < N)>
  ch_sint(const ch_sint<M>& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    base::operator=(other);
  }

  ch_sint(const ch_sint& other)
    : ch_sint(make_system_buffer(N, idname<ch_sint>())) {
    this->operator=(other);
  }

  ch_sint(ch_sint&& other) : buffer_(std::move(other.buffer_)) {}

  ch_sint& operator=(const ch_sint& other) {
    system_accessor::assign(*this, other);
    return *this;
  }

  ch_sint& operator=(ch_sint&& other) {
    system_accessor::move(*this, std::move(other));
    return *this;
  }

protected:

  const system_buffer_ptr& __buffer() const {
    return buffer_;
  }

  system_buffer_ptr buffer_;

  friend class system_accessor;
  template <unsigned M> friend class ch_sint;
};

///////////////////////////////////////////////////////////////////////////////

template <unsigned N = 32>
class ch_int : public ch_number_base<ch_int<N>> {
public:  
  static_assert(N != 0, "invalid size");
  using traits = logic_traits<N, true, ch_int, ch_sint<N>>;
  template <unsigned M> using size_cast = ch_int<M>;
  using base = ch_number_base<ch_int<N>>;
  using base::operator=;

  ch_int(const logic_buffer& buffer =
      logic_buffer(N, idname<ch_int>()))
    : buffer_(buffer) {
    assert(N == buffer.size());
  }

  template <typename U,
            CH_REQUIRE(std::is_integral_v<U>)>
  ch_int(const U& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  template <typename U,
            CH_REQUIRE(ch_width_v<U> <= N)>
  explicit ch_int(const ch_sbit_base<U>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=((const U&)other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_int(const ch_sbit<M>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_int(const ch_sint<M>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_int(const ch_suint<M>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  template <typename U,
            CH_REQUIRE(ch_width_v<U> <= N)>
  explicit ch_int(const ch_bit_base<U>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=((const U&)other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_int(const ch_bit<M>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  template <unsigned M,
            CH_REQUIRE(M <= N)>
  ch_int(const ch_uint<M>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  template <unsigned M,
            CH_REQUIRE(M < N)>
  ch_int(const ch_int<M>& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    base::operator=(other);
  }

  ch_int(const ch_int& other)
    : ch_int(logic_buffer(N, idname<ch_int>())) {
    CH_SOURCE_LOCATION(1);
    this->operator=(other);
  }

  ch_int(ch_int&& other) : buffer_(std::move(other.buffer_)) {}

  ch_int& operator=(const ch_int& other) {
    CH_SOURCE_LOCATION(1);
    logic_accessor::assign(*this, other);
    return *this;
  }

  ch_int& operator=(ch_int&& other) {
    CH_SOURCE_LOCATION(1);
    logic_accessor::move(*this, std::move(other));
    return *this;
  }

protected:

  const logic_buffer& __buffer() const {
    return buffer_;
  }

  logic_buffer buffer_;

  friend class logic_accessor;
  template <unsigned M> friend class ch_int;
};

}}

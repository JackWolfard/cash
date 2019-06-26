#pragma once

#include "bit.h"
#include "int.h"
#include "uint.h"

namespace ch {
namespace internal {

// read/write functions

template <typename T, typename U>
void ch_read(const T& obj,
             uint32_t src_offset,
             U* out,
             uint32_t dst_offset = 0,
             uint32_t length = ch_width_v<T>) {
  static_assert(is_system_type_v<T>, "invalid type");
  system_accessor::buffer(obj)->read(src_offset, out, sizeof(U), dst_offset, length);
}

template <typename T, typename U>
void ch_write(T& obj,
              uint32_t dst_offset,
              U* in,
              uint32_t src_offset = 0,
              uint32_t length = ch_width_v<T>) {
  static_assert(is_system_type_v<T>, "invalid type");
  system_accessor::buffer(obj)->write(dst_offset, in, sizeof(U), src_offset, length);
}

// slicing functions

template <typename R, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_slice(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  static_assert(ch_width_v<R> <= ch_width_v<T>, "invalid size");
  assert(start + ch_width_v<R> <= ch_width_v<T>);
  if constexpr (ch_width_v<T> == ch_width_v<R>) {
    CH_DBGCHECK(0 == start, "invalid offset");
    CH_UNUSED(start);
    return obj.template as<R>();
  } else
  if constexpr (is_logic_type_v<T>) {
    return logic_accessor::slice<R>(std::forward<T>(obj), start);
  } else {
    return system_accessor::slice<R>(std::forward<T>(obj), start);
  }
}

template <typename R, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_sliceref(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  static_assert(ch_width_v<R> <= ch_width_v<T>, "invalid size");
  assert(start + ch_width_v<R> <= ch_width_v<T>);
  if constexpr (ch_width_v<T> == ch_width_v<R>) {
    CH_DBGCHECK(0 == start, "invalid offset");
    CH_UNUSED(start);
    return obj.template as<R>();
  } else
  if constexpr (is_logic_type_v<T>) {
    return logic_accessor::sliceref<R>(std::forward<T>(obj), start);
  } else {
    return system_accessor::sliceref<R>(std::forward<T>(obj), start);
  }
}

template <typename R, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_aslice(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  return ch_slice<R>(std::forward<T>(obj), start * ch_width_v<R>);
}

template <typename R, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_asliceref(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  return ch_sliceref<R>(std::forward<T>(obj), start * ch_width_v<R>);
}

template <unsigned N, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_slice(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  if constexpr (ch_width_v<T> == N || !is_resizable_v<T>) {
    static_assert(ch_width_v<T> == N, "invalid size");
    CH_DBGCHECK(0 == start, "invalid offset");
    CH_UNUSED(start);
    return std::move(obj);
  } else {
    return ch_slice<ch_size_cast_t<T, N>>(std::forward<T>(obj), start);
  }
}

template <unsigned N, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_aslice(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  if constexpr (ch_width_v<T> == N || !is_resizable_v<T>) {
    static_assert(ch_width_v<T> == N, "invalid size");
    CH_DBGCHECK(0 == start, "invalid offset");
    CH_UNUSED(start);
    return std::move(obj);
  } else {
    return ch_aslice<ch_size_cast_t<T, N>>(std::forward<T>(obj), start);
  }
}

template <unsigned N, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_sliceref(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  if constexpr (ch_width_v<T> == N || !is_resizable_v<T>) {
    static_assert(ch_width_v<T> == N, "invalid size");
    CH_DBGCHECK(0 == start, "invalid offset");
    CH_UNUSED(start);
    return std::move(obj);
  } else {
    return ch_sliceref<ch_size_cast_t<T, N>>(std::forward<T>(obj), start);
  }
}

template <unsigned N, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_asliceref(T&& obj, size_t start = 0) {
  CH_SOURCE_LOCATION(1);
  if constexpr (ch_width_v<T> == N || !is_resizable_v<T>) {
    static_assert(ch_width_v<T> == N, "invalid size");
    CH_DBGCHECK(0 == start, "invalid offset");
    CH_UNUSED(start);
    return std::move(obj);
  } else {
    return ch_asliceref<ch_size_cast_t<T, N>>(std::forward<T>(obj), start);
  }
}

// rotate functions

template <typename T>
inline auto ch_rotl(const ch_bit_base<T>& lhs, uint32_t rhs) {
  CH_SOURCE_LOCATION(1);
  auto& _lhs = reinterpret_cast<const T&>(lhs);
  return make_logic_type<T>(createRotateNode(get_lnode(_lhs), rhs, false));
}

template <typename T>
inline auto ch_rotr(const ch_bit_base<T>& lhs, uint32_t rhs) {
  CH_SOURCE_LOCATION(1);
  auto& _lhs = reinterpret_cast<const T&>(lhs);
  return make_logic_type<T>(createRotateNode(get_lnode(_lhs), rhs, true));
}

// padding function

template <unsigned N, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_pad(T&& obj) {
  CH_SOURCE_LOCATION(1);
  if constexpr (0 == N || !is_resizable_v<T>) {
    static_assert(0 == N, "invalid size");
    return std::move(obj);
  } else {
    return ch_size_cast_t<T, (ch_width_v<T> + N)>(std::forward<T>(obj));
  }
}

// resize function

template <typename R, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_resize(T&& obj) {
  CH_SOURCE_LOCATION(1);
  if constexpr (ch_width_v<T> <= ch_width_v<R>) {
    return ch_pad<ch_width_v<R>-ch_width_v<T>>(std::forward<T>(obj)).template as<R>();
  } else {
    return ch_slice<R>(std::forward<T>(obj), 0);
  }
}

template <unsigned M, typename T,
          CH_REQUIRE(is_data_type_v<T>)>
auto ch_resize(T&& obj) {
  CH_SOURCE_LOCATION(1);
  if constexpr (ch_width_v<T> <= M) {
    return ch_pad<M-ch_width_v<T>>(std::forward<T>(obj));
  } else {
    return ch_slice<M>(std::forward<T>(obj), 0);
  }
}

// shuffle function

template <unsigned M, typename T>
auto ch_shuffle(const ch_bit_base<T>& obj, const std::array<unsigned, M>& indices) {
  static_assert(0 == (ch_width_v<T> % M), "invalid indices size");
  CH_SOURCE_LOCATION(1);
  auto node = createShuffleNode(get_lnode(reinterpret_cast<const T&>(obj)),
                                std::vector<unsigned>(indices.begin(), indices.end()));
  return make_logic_type<T>(node);
}

// tie function

template <typename... Args>
class tie_impl {
public:
  tie_impl(Args&... args) : args_(args...) {}

  template <typename U>
  void operator=(const U& other) {
    static_assert(is_bit_convertible_v<U, ch_width_v<Args...>>, "invalid type");
    this->assign(to_logic<ch_width_v<Args...>>(other), std::index_sequence_for<Args...>());
  }

protected:

  template <typename U, typename V>
  void assign_impl(uint32_t src_offset, const U& src, V& dst) {
    dst = ch_slice<V>(src, src_offset);
  }

  template <typename U, typename V0, typename... Vs>
  void assign_impl(uint32_t src_offset, const U& src, V0& dst0, Vs&... dsts) {
    this->assign_impl(src_offset, src, dst0);
    this->assign_impl(src_offset + ch_width_v<V0>, src, dsts...);
  }

  template <typename U, size_t... I>
  void assign(const U& src, std::index_sequence<I...>) {
    this->assign_impl(0, src, std::get<sizeof...(Args) - 1 - I>(args_)...);
  }

  std::tuple<Args&...> args_;
};

template <typename... Args>
  auto ch_tie(Args&... args) {
  static_assert((is_logic_type_v<Args> && ...), "invalid type for argument");
  CH_SOURCE_LOCATION(1);
  return tie_impl(args...);
}

// concatenation function

template <typename O, typename I>
void cat_impl(O& inout, uint32_t dst_offset, const I& arg) {
  logic_accessor::write(inout, dst_offset - ch_width_v<I>, arg, 0, ch_width_v<I>);
}

template <typename O, typename I0, typename... Is>
void cat_impl(O& inout, uint32_t dst_offset, const I0& arg0, const Is&... args) {
  cat_impl(inout, dst_offset, arg0);
  cat_impl(inout, dst_offset - ch_width_v<I0>, args...);
}

template <typename R, typename... Args>
auto ch_cat(const Args&... args) {
  static_assert((is_data_type_v<Args> && ...), "invalid argument type");
  static constexpr unsigned N = (ch_width_v<Args> + ...);
  static_assert(ch_width_v<R> == N, "size mismatch");
  CH_SOURCE_LOCATION(1);
  R ret(make_logic_buffer(N, "cat"));
  cat_impl(ret, N, to_logic<ch_width_v<Args>>(args)...);
  return ret;
}

template <typename... Args>
auto ch_cat(const Args&... args) {
  static constexpr unsigned N = (ch_width_v<Args> + ...);
  CH_SOURCE_LOCATION(1);
  return ch_cat<ch_bit<N>>(args...);
}

// duplicate function

template <unsigned N, typename T>
auto ch_dup(const T& obj) {
  static_assert(is_logic_type_v<T>, "invalid type");
  CH_SOURCE_LOCATION(1);
  ch_bit<ch_width_v<T> * N> out(make_logic_buffer(ch_width_v<T> * N, "dup"));
  for (unsigned i = 0; i < N; ++i) {
    ch_asliceref<ch_width_v<T>>(out, i) = obj;
  }
  return out;
}

// cast function

template <typename R, typename T>
auto ch_as(const T& obj) {
  static_assert(is_data_type_v<T>, "invalid type");
  CH_SOURCE_LOCATION(1);
  return obj.template as<R>();
}

// reference function

template <typename T>
auto ch_ref(T& obj) {
  static_assert(is_logic_type_v<T>, "invalid type");
  CH_SOURCE_LOCATION(1);
  return obj.ref();
}

// clone function

template <typename T>
auto ch_clone(const T& obj) {
  static_assert(is_logic_type_v<T>, "invalid type");
  CH_SOURCE_LOCATION(1);
  return obj.clone();
}

// min/max functions

template <typename U, typename V>
auto ch_min(const U& lhs, const V& rhs) {
  CH_SOURCE_LOCATION(1);
  return ch_sel(lhs < rhs, lhs, rhs);
}

template <typename U, typename V>
auto ch_max(const U& lhs, const V& rhs) {
  CH_SOURCE_LOCATION(1);
  return ch_sel(lhs > rhs, lhs, rhs);
}

// abs function

template <unsigned N>
auto ch_abs(const ch_int<N>& obj) {
  CH_SOURCE_LOCATION(1);
  return ch_sel(obj[N-1], -obj, obj);
}

}
}
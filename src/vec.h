#pragma once

#include "port.h"

namespace ch {
namespace internal {

template <typename T, unsigned N, typename Enable = void>
class const_vec {};

template <typename T, unsigned N, typename Enable = void>
class ch_vec {};

template <typename T, unsigned N>
class const_vec_base {
public:
  const_vec_base() {}
  const_vec_base(const const_vec_base& rhs) : items_(rhs.items_) {}
  const_vec_base(const_vec_base&& rhs) : items_(std::move(rhs.items_)) {}

  const T& at(size_t i) const {
    CH_CHECK(i < N, "invalid subscript index");
    return items_[i];
  }

  const T& operator[](size_t i) const {
    return this->at(i);
  }

  auto front() const {
    return items_.front();
  }

  auto back() const {
    return items_.back();
  }

  auto begin() const {
    return items_.cbegin();
  }

  auto cbegin() const {
    return items_.cbegin();
  }

  auto end() const {
    return items_.cend();
  }

  auto cend() const {
    return items_.cend();
  }

  auto rbegin() const {
    return items_.crbegin();
  }

  auto crbegin() const {
    return items_.crbegin();
  }

  auto rend() const {
    return items_.crend();
  }

  auto crend() const {
    return items_.crend();
  }

  auto size() const {
    return items_.size();
  }

  auto empty() const {
    return items_.empty();
  }

protected:

  template <typename... Ts,
            CH_REQUIRES(sizeof...(Ts) == N && are_all_cast_convertible<T, Ts...>::value)>
  explicit const_vec_base(Ts&&... values)
    : items_{(T(std::forward<Ts>(values)))...}
  {}
  
  std::array<T, N> items_;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class vec_base : public const_vec_base<T, N> {
public:
  using base = const_vec_base<T, N>;
  using base::operator [];
  using base::items_;

  vec_base() {}
  vec_base(const vec_base& rhs) : base(rhs) {}
  vec_base(vec_base&& rhs) : base(std::move(rhs)) {}

  vec_base& operator=(const vec_base& rhs) {
    items_ = rhs.items_;
    return *this;
  }

  vec_base& operator=(vec_base&& rhs) {
    items_ = std::move(rhs.items_);
    return *this;
  }

  vec_base& operator=(const const_vec_base<T, N>& rhs) {
    items_ = rhs.items_;
    return *this;
  }

  T& at(size_t i) {
    CH_CHECK(i < N, "invalid subscript index");
    return items_[i];
  }

  T& operator[](size_t i) {
    return this->at(i);
  }

  auto front() {
    return items_.front();
  }

  auto back() {
    return items_.back();
  }

  auto begin() {
    return items_.begin();
  }

  auto end() {
    return items_.end();
  }

  auto rbegin() {
    return items_.rbegin();
  }

  auto rend() {
    return items_.rend();
  }

  template <typename U>
  void fill(const U& value) {
    for (unsigned i = 0; i < N; ++i) {
      items_[i] = value;
    }
  }

protected:

  vec_base(const const_vec_base<T, N>& rhs) : base(rhs) {}

  template <typename... Ts,
            CH_REQUIRES(sizeof...(Ts) == N && are_all_cast_convertible<T, Ts...>::value)>
  explicit vec_base(Ts&&... values) : base(std::forward<Ts>(values)...) {}
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class const_vec<T, N, typename std::enable_if_t<is_logic_type<T>::value>>
    : public const_vec_base<T, N> {
public:
  static_assert(is_logic_type<T>::value, "invalid value type");
  static_assert(!is_const_type<T>::value, "invalid value type");
  using traits = logic_traits<N * bitwidth_v<T>,
                              const_vec,
                              const_vec,
                              ch_vec<T, N>,
                              const_vec<scalar_type_t<T>, N>>;
  using base = const_vec_base<T, N>;
  using base::operator [];
  using base::items_;

  const_vec(const bit_buffer& buffer = bit_buffer(traits::bitwidth))
    : const_vec(buffer, std::make_index_sequence<N>())
  {}

  const_vec(const const_vec& rhs)
    : const_vec(bit_accessor::cloneBuffer(rhs))
  {}

  const_vec(const_vec&& rhs) : base(std::move(rhs)) {}

  explicit const_vec(const ch_scalar<traits::bitwidth>& rhs)
    : const_vec(bit_buffer(scalar_accessor::get_data(rhs)))
  {}

  template <typename __T__, CH_REQUIRES(std::is_integral_v<__T__>)>
  explicit const_vec(__T__ rhs)
    : const_vec(bit_buffer(bitvector(traits::bitwidth, rhs)))
  {}

  template <typename... Vs,
            CH_REQUIRES(sizeof...(Vs) == N && are_all_cast_convertible<T, Vs...>::value)>
  explicit const_vec(Vs&&... values)
    : const_vec() {
    this->init(std::forward<Vs>(values)...);
  }

  CH_LOGIC_READONLY_INTERFACE(const_vec)

protected:

  template <typename V>
  void init(const V& value) {
    bit_accessor::copy(
        items_[0],
        static_cast<aggregate_init_cast_t<T, V>>(value));
  }

  template <typename V0, typename... Vs>
  void init(const V0& value0, const Vs&... values) {
    bit_accessor::copy(
        items_[sizeof...(Vs)],
        static_cast<aggregate_init_cast_t<T, V0>>(value0));
    this->init(values...);
  }

  template <std::size_t...Is>
  explicit const_vec(const bit_buffer& buffer, std::index_sequence<Is...>)
    : base(bit_buffer(bitwidth_v<T>, buffer, Is * bitwidth_v<T>)...)
  {}

  const bit_buffer_ptr& get_buffer() const {
    return bit_accessor::get_buffer(items_[0])->get_source();
  }

  bit_buffer_ptr& get_buffer() {
    return bit_accessor::get_buffer(items_[0])->get_source();
  }

  friend class bit_accessor;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class ch_vec<T, N, typename std::enable_if_t<is_logic_type<T>::value>>
    : public const_vec<T, N> {
public:
  static_assert(is_logic_type<T>::value, "invalid value type");
  static_assert(!is_const_type<T>::value, "invalid value type");
  using traits = logic_traits<N * bitwidth_v<T>,
                              ch_vec,
                              const_vec<T, N>,
                              ch_vec,
                              ch_vec<scalar_type_t<T>, N>>;
  using base = const_vec<T, N>;
  using base::operator [];
  using base::items_;

  ch_vec(const bit_buffer& buffer = bit_buffer(traits::bitwidth)) : base(buffer) {}
  ch_vec(const const_vec<T, N>& rhs) : base(rhs) {}
  ch_vec(const ch_vec& rhs) : base(rhs) {}
  ch_vec(ch_vec&& rhs) : base(std::move(rhs)) {}

  template <typename U>
  explicit ch_vec(const ch_vec<U, N>& rhs) : base(rhs ) {}

  explicit ch_vec(const ch_scalar<traits::bitwidth>& rhs) : base(rhs) {}

  template <typename __T__, CH_REQUIRES(std::is_integral_v<__T__>)>
  explicit ch_vec(__T__ rhs) : base(rhs) {}

  template <typename... Vs,
            CH_REQUIRES(sizeof...(Vs) == N && are_all_cast_convertible<T, Vs...>::value)>
  explicit ch_vec(Vs&&... values) : base(std::forward<Vs>(values)...) {}

  ch_vec& operator=(const ch_vec& rhs) {
    bit_accessor::copy(*this, rhs);
    return *this;
  }

  ch_vec& operator=(ch_vec&& rhs) {
    bit_accessor::move(*this, std::move(rhs));
    return *this;
  }

  ch_vec& operator=(const const_vec<T, N>& rhs) {
    bit_accessor::copy(*this, rhs);
    return *this;
  }

  T& at(size_t i) {
    CH_CHECK(i < N, "invalid subscript index");
    return items_[i];
  }

  T& operator[](size_t i) {
    return this->at(i);
  }

  auto front() {
    return items_.front();
  }

  auto back() {
    return items_.back();
  }

  auto begin() {
    return items_.begin();
  }

  auto end() {
    return items_.end();
  }

  auto rbegin() {
    return items_.rbegin();
  }

  auto rend() {
    return items_.rend();
  }

  template <typename U>
  void fill(const U& value) {
    for (unsigned i = 0; i < N; ++i) {
      items_[i] = value;
    }
  }

  CH_LOGIC_WRITABLE_INTERFACE(ch_vec)
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class const_vec<T, N, typename std::enable_if_t<is_scalar_type<T>::value>>
    : public const_vec_base<T, N> {
public:
  static_assert(is_scalar_type<T>::value, "invalid value type");
  static_assert(!is_const_type<T>::value, "invalid value type");
  using traits = scalar_traits<N * bitwidth_v<T>,
                               const_vec,
                               const_vec,
                               ch_vec<T, N>,
                               const_vec<logic_type_t<T>, N>>;
  using base = const_vec_base<T, N>;
  using base::operator [];
  using base::items_;

  const_vec(const scalar_buffer& buffer = scalar_buffer(traits::bitwidth))
    : const_vec(buffer, std::make_index_sequence<N>())
  {}

  const_vec(const const_vec& rhs)
    : const_vec(scalar_accessor::cloneBuffer(rhs))
  {}

  const_vec(const_vec&& rhs) : base(std::move(rhs)) {}

  template <typename U>
  explicit const_vec(const const_vec<U, N>& rhs)
    : const_vec(scalar_accessor::cloneBuffer(rhs))
  {}

  explicit const_vec(const ch_scalar<traits::bitwidth>& rhs)
    : const_vec(scalar_accessor::cloneBuffer(rhs))
  {}

  template <typename __T__, CH_REQUIRES(std::is_integral_v<__T__>)>
  explicit const_vec(__T__ rhs)
    : const_vec(scalar_buffer(bitvector(traits::bitwidth, rhs)))
  {}

  template <typename... Vs,
            CH_REQUIRES(sizeof...(Vs) == N && are_all_cast_convertible<T, Vs...>::value)>
  explicit const_vec(Vs&&... values) : const_vec() {
    this->init(std::forward<Vs>(values)...);
  }

  CH_SCALAR_READONLY_INTERFACE(const_vec)

protected:

  template <typename V>
  void init(const V& value) {
    scalar_accessor::copy(
        items_[0],
        static_cast<aggregate_init_cast_t<T, V>>(value));
  }

  template <typename V0, typename... Vs>
  void init(const V0& value0, const Vs&... values) {
    scalar_accessor::copy(
        items_[sizeof...(Vs)],
        static_cast<aggregate_init_cast_t<T, V0>>(value0));
    this->init(values...);
  }

  template <std::size_t...Is>
  explicit const_vec(const scalar_buffer& buffer, std::index_sequence<Is...>)
    : base(scalar_buffer(bitwidth_v<T>, buffer, Is * bitwidth_v<T>)...)
  {}

  const scalar_buffer_ptr& get_buffer() const {
    return scalar_accessor::get_buffer(items_[0])->get_source();
  }

  scalar_buffer_ptr& get_buffer() {
    return scalar_accessor::get_buffer(items_[0])->get_source();
  }

  friend class scalar_accessor;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class ch_vec<T, N, typename std::enable_if_t<is_scalar_type<T>::value>>
    : public const_vec<T, N> {
public:
  static_assert(is_scalar_type<T>::value, "invalid value type");
  static_assert(!is_const_type<T>::value, "invalid value type");
  using traits = scalar_traits<N * bitwidth_v<T>,
                               ch_vec,
                               const_vec<T, N>,
                               ch_vec,
                               ch_vec<logic_type_t<T>, N>>;
  using base = const_vec<T, N>;
  using base::operator [];
  using base::items_;

  ch_vec(const scalar_buffer& buffer = scalar_buffer(traits::bitwidth)) : base(buffer) {}
  ch_vec(const ch_vec& rhs) : base(rhs) {}
  ch_vec(ch_vec&& rhs) : base(std::move(rhs)) {}

  template <typename U>
  explicit ch_vec(const ch_vec<U, N>& rhs) : base(rhs) {}

  explicit ch_vec(const ch_scalar<traits::bitwidth>& rhs) : base(rhs) {}

  template <typename __T__, CH_REQUIRES(std::is_integral_v<__T__>)>
  explicit ch_vec(__T__ rhs) : base(rhs) {}

  template <typename... Vs,
            CH_REQUIRES(sizeof...(Vs) == N && are_all_cast_convertible<T, Vs...>::value)>
  explicit ch_vec(Vs&&... values) : base(std::forward<Vs>(values)...) {}

  ch_vec& operator=(const ch_vec& rhs) {
    scalar_accessor::copy(*this, rhs);
    return *this;
  }

  ch_vec& operator=(ch_vec&& rhs) {
    scalar_accessor::move(*this, std::move(rhs));
    return *this;
  }

  T& at(size_t i) {
    CH_CHECK(i < N, "invalid subscript index");
    return items_[i];
  }

  T& operator[](size_t i) {
    return this->at(i);
  }

  auto front() {
    return items_.front();
  }

  auto back() {
    return items_.back();
  }

  auto begin() {
    return items_.begin();
  }

  auto end() {
    return items_.end();
  }

  auto rbegin() {
    return items_.rbegin();
  }

  auto rend() {
    return items_.rend();
  }

  template <typename U>
  void fill(const U& value) {
    for (unsigned i = 0; i < N; ++i) {
      items_[i] = value;
    }
  }

  CH_SCALAR_WRITABLE_INTERFACE(ch_vec)
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class ch_vec_buffer_io : public scalar_buffer_io {
public:
  using base = scalar_buffer_io;

  ch_vec_buffer_io(ch_vec<T, N>& rhs)
    : ch_vec_buffer_io(rhs, std::make_index_sequence<N>())
  {}

  void read(uint32_t dst_offset,
            void* out,
            uint32_t out_cbsize,
            uint32_t src_offset,
            uint32_t length) const override {
    for (auto& item : items_) {
      if (src_offset < bitwidth_v<T>) {
        size_t len = std::min<size_t>(length, bitwidth_v<T> - src_offset);
        item.read(dst_offset, out, out_cbsize, src_offset, len);
        length -= len;
        if (0 == length)
          return;
        dst_offset += len;
        src_offset = bitwidth_v<T>;
      }
      src_offset -= bitwidth_v<T>;
    }
  }

  void write(uint32_t dst_offset,
             const void* in,
             uint32_t in_cbsize,
             uint32_t src_offset,
             uint32_t length) override {
    for (auto& item : items_) {
      if (dst_offset < bitwidth_v<T>) {
        size_t len = std::min<size_t>(length, bitwidth_v<T> - dst_offset);
        item.write(dst_offset, in, in_cbsize, src_offset, len);
        length -= len;
        if (0 == length)
          return;
        src_offset += len;
        dst_offset = bitwidth_v<T>;
      }
      dst_offset -= bitwidth_v<T>;
    }
  }

protected:

  template <std::size_t...Is>
  explicit ch_vec_buffer_io(ch_vec<T, N>& rhs, std::index_sequence<Is...>)
    : base(N * bitwidth_v<T>)
    , items_{buffer_type_t<T>(rhs[Is])...}
  {}

  std::array<buffer_type_t<T>, N> items_;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
using resolve_const_vec = std::conditional_t<is_const_type<T>::value,
                              const_vec<value_type_t<T>, N>, ch_vec<T, N>>;

template <typename T, unsigned N>
class ch_vec<T, N, typename std::enable_if_t<is_io_type<T>::value>>
    : public vec_base<T, N> {
public:
  static_assert(is_io_type<T>::value, "invalid value type");
  using traits = io_traits<ch_vec,
                          direction_v<T>,
                          ch_vec<flip_type_t<T>, N>,
                          ch_vec_buffer_io<T, N>,
                          resolve_const_vec<logic_type_t<T>, N>>;

  using base = vec_base<T, N>;
  using base::operator [];
  using base::items_;

  ch_vec(const std::string& name = "io")
    : ch_vec(name, std::make_index_sequence<N>())
  {}

  ch_vec(const ch_vec<flip_type_t<T>, N>& rhs)
    : ch_vec(rhs, std::make_index_sequence<N>())
  {}

  ch_vec(const ch_vec& rhs) : base(rhs) {}

  ch_vec(ch_vec&& rhs) : base(std::move(rhs)) {}

  void operator()(typename traits::flip_type& rhs) {
    for (unsigned i = 0, n = items_.size(); i < n; ++i) {
      items_[i](rhs[i]);
    }
  }

protected:

  ch_vec& operator=(const ch_vec& rhs) = delete;
  ch_vec& operator=(ch_vec&& rhs) = delete;

  template <std::size_t...Is>
  explicit ch_vec(const std::string& name, std::index_sequence<Is...>)
    : base((fstring("%s_%d", name.c_str(), Is))...)
  {}

  template <std::size_t...Is>
  explicit ch_vec(const ch_vec<flip_type_t<T>, N>& rhs, std::index_sequence<Is...>)
    : base(rhs[Is]...)
  {}
};

}
}

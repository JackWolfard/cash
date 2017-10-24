#pragma once

#include "bitvector.h"
#include "arithm.h"

namespace ch {
namespace internal {

CH_DEF_SFINAE_CHECK(has_bitsize, T::bitwidth != 0);
static_assert(!has_bitsize<int>::value, ":-(");

///////////////////////////////////////////////////////////////////////////////

struct non_bitsize_type {
  static constexpr unsigned bitwidth = 0;
};

template <typename T0, typename T1>
struct deduce_type_impl {
  using D0 = std::decay_t<T0>;
  using D1 = std::decay_t<T1>;
  using U0 = std::conditional_t<has_bitsize<D0>::value, D0, non_bitsize_type>;
  using U1 = std::conditional_t<has_bitsize<D1>::value, D1, non_bitsize_type>;
  using type = std::conditional_t<
    (U0::bitwidth != 0) && (U1::bitwidth != 0),
    std::conditional_t<(U0::bitwidth != U1::bitwidth), non_bitsize_type, U0>,
    std::conditional_t<(U0::bitwidth != 0), U0, U1>>;
};

template <typename... Ts>
struct deduce_type;

template <typename T0, typename T1>
struct deduce_type<T0, T1> {
  using type = typename deduce_type_impl<T0, T1>::type;
};

template <typename T0, typename T1, typename... Ts>
struct deduce_type<T0, T1, Ts...> {
  using type = typename deduce_type<typename deduce_type_impl<T0, T1>::type, Ts...>::type;
};

template <typename... Ts>
using deduce_type_t = typename deduce_type<Ts...>::type;

template <typename T0, typename T1>
struct deduce_first_type_impl {
  using D0 = std::decay_t<T0>;
  using D1 = std::decay_t<T1>;
  using U0 = std::conditional_t<has_bitsize<D0>::value, D0, non_bitsize_type>;
  using U1 = std::conditional_t<has_bitsize<D1>::value, D1, non_bitsize_type>;
  using type = std::conditional_t<(U0::bitwidth != 0), U0, U1>;
};

template <typename T0, typename T1>
using deduce_first_type_t = typename deduce_first_type_impl<T0, T1>::type;

///////////////////////////////////////////////////////////////////////////////

template <unsigned N> class ch_bit;

template <unsigned N> class ch_scalar;

template <unsigned N> class const_scalar_slice;

template <unsigned N> class ch_scalar_slice;

template <typename ScalarType, typename LogicType>
struct scalar_traits {
  using scalar_type = ScalarType;
  using logic_type = LogicType;
};

template <typename T>
using scalar_type_t = typename std::decay_t<T>::traits::scalar_type;

template <typename T>
struct is_scalar_traits : std::false_type {};

template <typename ScalarType, typename LogicType>
struct is_scalar_traits<scalar_traits<ScalarType, LogicType>> : std::true_type {};

CH_DEF_SFINAE_CHECK(is_scalar_type, is_scalar_traits<typename std::decay_t<T>::traits>::value);

CH_DEF_SFINAE_CHECK(is_scalar_compatible, (std::is_same<ch_scalar<T::bitwidth>, T>::value
                                        || std::is_base_of<ch_scalar<T::bitwidth>, T>::value));

template <typename... Ts>
using deduce_ch_scalar_t = std::conditional_t<
  is_scalar_compatible<deduce_type_t<Ts...>>::value, deduce_type_t<Ts...>, non_bitsize_type>;

template <typename T, unsigned N = std::decay_t<T>::bitwidth>
using is_scalar_convertible = is_cast_convertible<ch_scalar<N>, T>;

///////////////////////////////////////////////////////////////////////////////

class scalar_buffer {
public:
  scalar_buffer(unsigned size);

  scalar_buffer(const scalar_buffer& rhs);

  scalar_buffer(scalar_buffer&& rhs);

  scalar_buffer(const bitvector& data);

  scalar_buffer(bitvector&& data);

  scalar_buffer(unsigned size, const scalar_buffer& buffer, unsigned offset = 0);

  scalar_buffer& operator=(const scalar_buffer& rhs);

  scalar_buffer& operator=(scalar_buffer&& rhs);

  void set_data(const bitvector& data);

  const bitvector& get_data() const;

  void read(uint32_t dst_offset, void* out, uint32_t out_cbsize, uint32_t src_offset, uint32_t length) const;

  void write(uint32_t dst_offset, const void* in, uint32_t in_cbsize, uint32_t src_offset, uint32_t length);

  const auto& get_source() const {
    return source_;
  }

  auto& get_source() {
    return source_;
  }

  auto get_offset() const {
    return offset_;
  }

  auto get_size() const {
    return size_;
  }

  bool is_slice() const;

private:
  using source_t = std::pair<bitvector, uint64_t>;
  std::shared_ptr<source_t> source_;
  unsigned offset_;
  unsigned size_;
  mutable bitvector cache_;
  mutable uint64_t version_;
};

struct scalar_accessor {
  template <typename T>
  static const auto& get_buffer(const T& obj) {
    return obj.get_buffer();
  }

  template <typename T>
  static auto& get_buffer(T& obj) {
    return obj.get_buffer();
  }

  template <typename T>
  static const auto& get_data(const T& obj) {
    return obj.get_buffer().get_data();
  }

  template <typename U, typename V,
            CH_REQUIRES(U::bitwidth == V::bitwidth)>
  static void copy(U& dst, const V& src) {
    auto& d = dst.get_buffer().get_source();
    auto d_offset = dst.get_buffer().get_offset();
    const auto& s = src.get_buffer().get_source();
    auto s_offset = src.get_buffer().get_offset();
    d->first.copy(d_offset, s->first, s_offset, U::bitwidth);
    ++d->second;
  }

  template <typename U, typename V,
            CH_REQUIRES(U::bitwidth == V::bitwidth)>
  static void move(U& dst, V&& src) {
    if (dst.get_buffer().is_slice()) {
      copy(dst, src);
    } else {
      dst = std::move(src);
    }
  }

  template <typename T>
  static auto cloneBuffer(const T& obj) {
    scalar_buffer ret(T::bitwidth);
    auto& s = obj.get_buffer().get_source();
    auto s_offset = obj.get_buffer().get_offset();
    ret.write(0, s->first.get_words(), s->first.get_cbsize(), s_offset, T::bitwidth);
    return ret;
  }

  template <typename D, typename T>
  static auto cast(const T& obj) {
    return D(scalar_buffer(T::bitwidth, obj.get_buffer(), 0));
  }
};

///////////////////////////////////////////////////////////////////////////////

#define CH_SCALAR_FRIEND_OPS(i, x) \
  CH_FRIEND_OP_EQ(, const ch_scalar&, x) \
  CH_FRIEND_OP_NE(, const ch_scalar&, x) \
  CH_FRIEND_OP_LT(, const ch_scalar&, x) \
  CH_FRIEND_OP_LE(, const ch_scalar&, x) \
  CH_FRIEND_OP_GT(, const ch_scalar&, x) \
  CH_FRIEND_OP_GE(, const ch_scalar&, x) \
  CH_FRIEND_OP_AND(, const ch_scalar&, x) \
  CH_FRIEND_OP_OR(, const ch_scalar&, x) \
  CH_FRIEND_OP_XOR(, const ch_scalar&, x) \
  CH_FRIEND_OP_ADD(, const ch_scalar&, x) \
  CH_FRIEND_OP_SUB(, const ch_scalar&, x) \
  CH_FRIEND_OP_MULT(, const ch_scalar&, x) \
  CH_FRIEND_OP_DIV(, const ch_scalar&, x) \
  CH_FRIEND_OP_MOD(, const ch_scalar&, x) \
  CH_FRIEND_OP_SLL(, const ch_scalar&, x) \
  CH_FRIEND_OP_SRL(, const ch_scalar&, x)

#define CH_SCALAR_OP_TYPES \
  int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t

#define CH_SCALAR_TYPE_INTERFACE(type) \
  template <typename R> \
  R as() const { \
    return ch::internal::scalar_accessor::cast<R>(*this); \
  } \
  ch_scalar<type::bitwidth> asScalar() const { \
    return this->as<ch_scalar<type::bitwidth>>(); \
  } \
  void read(uint32_t dst_offset, void* out, uint32_t out_cbsize, uint32_t src_offset = 0, uint32_t length = bitwidth) const { \
    this->get_buffer().read(dst_offset, out, out_cbsize, src_offset, length); \
  } \
  void write(uint32_t dst_offset, const void* in, uint32_t in_cbsize, uint32_t src_offset = 0, uint32_t length = bitwidth) { \
    this->get_buffer().write(dst_offset, in, in_cbsize, src_offset, length); \
  }

template <unsigned N>
class ch_scalar {
public:
  static constexpr unsigned bitwidth = N;
  using traits = scalar_traits<ch_scalar, ch_bit<N>>;

  ch_scalar() : buffer_(N) {}

  ch_scalar(const ch_scalar& rhs) : buffer_(scalar_accessor::cloneBuffer(rhs)) {}

  ch_scalar(ch_scalar&& rhs) : buffer_(std::move(rhs.buffer_)) {}

  ch_scalar(const const_scalar_slice<N>& rhs)
    : buffer_(scalar_accessor::cloneBuffer(rhs)) {}

  explicit ch_scalar(const scalar_buffer& buffer) : buffer_(buffer) {
    assert(N == buffer.get_size());
  }

  template <typename U,
            CH_REQUIRES(is_scalar_type<U>::value),
            CH_REQUIRES(N == U::bitwidth)>
  explicit ch_scalar(const U& rhs) : buffer_(scalar_accessor::cloneBuffer(rhs)) {}

  template <typename U, CH_REQUIRES(is_bitvector_value<U>::value || std::is_enum<U>::value)>
  explicit ch_scalar(const U& value) : buffer_(bitvector(N, value)) {}

  ch_scalar& operator=(const ch_scalar& rhs) {
    scalar_accessor::copy(*this, rhs);
    return *this;
  }

  ch_scalar& operator=(ch_scalar&& rhs) {
    scalar_accessor::move(*this, std::move(rhs));
    return *this;
  }

  ch_scalar& operator=(const const_scalar_slice<N>& rhs) {
    scalar_accessor::copy(*this, rhs);
    return *this;
  }

  template <typename U,
            CH_REQUIRES(is_scalar_type<U>::value),
            CH_REQUIRES(N == U::bitwidth)>
  ch_scalar& operator=(const U& rhs) {
    scalar_accessor::copy(*this, rhs);
    return *this;
  }

  template <typename U, CH_REQUIRES(is_integral_or_enum<U>::value)>
  ch_scalar& operator=(U value) {
    buffer_.set_data(bitvector(N, value));
    return *this;
  }

  const auto operator[](size_t index) const {
    return const_scalar_slice<1>(buffer_, index);
  }

  auto operator[](size_t index) {
    return ch_bit<1>(scalar_buffer(1, buffer_, index));
  }

  template <unsigned M>
  const auto slice(size_t start = 0) const {
    return const_scalar_slice<M>(buffer_, start);
  }

  template <unsigned M>
  auto slice(size_t start = 0) {
    return ch_bit<M>(scalar_buffer(M, buffer_, start));
  }

  template <typename U, CH_REQUIRES(can_bitvector_cast<U>::value)>
  explicit operator U() const {
    return static_cast<U>(buffer_.get_data());
  }

  auto operator==(const ch_scalar& rhs) const {
    return (buffer_.get_data() == rhs.buffer_.get_data());
  }

  auto operator!=(const ch_scalar& rhs) const {
    return !(buffer_.get_data() == rhs.buffer_.get_data());
  }

  auto operator<(const ch_scalar& rhs) const {
    return (buffer_.get_data() < rhs.buffer_.get_data());
  }

  auto operator>=(const ch_scalar& rhs) const {
    return !(buffer_.get_data() < rhs.buffer_.get_data());
  }

  auto operator>(const ch_scalar& rhs) const {
    return (rhs.buffer_.get_data() < buffer_.get_data());
  }

  auto operator<=(const ch_scalar& rhs) const {
    return !(rhs.buffer_.get_data() < rhs.buffer_.get_data());
  }

  const auto operator!() const {
    return (ch_scalar(0x0) == *this);
  }

  const auto operator~() const {
    bitvector ret(N);
    Inverse(ret, buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator&(const ch_scalar& rhs) const {
    bitvector ret(N);
    And(ret, buffer_.get_data(), rhs.buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator|(const ch_scalar& rhs) const {
    bitvector ret(N);
    Or(ret, buffer_.get_data(), rhs.buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator^(const ch_scalar& rhs) const {
    bitvector ret(N);
    Xor(ret, buffer_.get_data(), rhs.buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator-() const {
    bitvector ret(N);
    Negate(ret, buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator+(const ch_scalar& rhs) const {
    bitvector ret(N);
    Add(ret, buffer_.get_data(), rhs.buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator-(const ch_scalar& rhs) const {
    bitvector ret(N);
    Sub(ret, buffer_.get_data(), rhs.buffer_.get_data());
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator<<(const ch_scalar& rhs) const {
    bitvector ret(N);
    auto shift = rhs.buffer_.get_data();
    CH_CHECK(shift.find_last() <= 31, "shift amount out of range!");
    SLL(ret, buffer_.get_data(), shift.get_word(0));
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  const auto operator>>(const ch_scalar& rhs) const {
    bitvector ret(N);
    auto shift = rhs.buffer_.get_data();
    CH_CHECK(shift.find_last() <= 31, "shift amount out of range!");
    SRL(ret, buffer_.get_data(), shift.get_word(0));
    return ch_scalar(scalar_buffer(std::move(ret)));
  }

  CH_SCALAR_TYPE_INTERFACE(ch_scalar);

protected:

  const scalar_buffer& get_buffer() const {
    return buffer_;
  }

  scalar_buffer& get_buffer() {
    return buffer_;
  }

  scalar_buffer buffer_;

  friend class scalar_accessor;

  friend std::ostream& operator<<(std::ostream& out, const ch_scalar& rhs) {
    return out << scalar_accessor::get_data(rhs);
  }

  CH_FOR_EACH(CH_SCALAR_FRIEND_OPS, CH_SEP_SPACE, CH_SCALAR_OP_TYPES)
};

///////////////////////////////////////////////////////////////////////////////

template <unsigned N>
class const_scalar_slice {
public:
  static constexpr unsigned bitwidth = N;
  using traits = scalar_traits<const_scalar_slice, ch_bit<N>>;

  const_scalar_slice(const scalar_buffer& buffer, unsigned start = 0)
    : buffer_(scalar_buffer(N, buffer, start)) {
    assert(N + start <= buffer.get_size());
  }

  const_scalar_slice(const const_scalar_slice& rhs)
    : buffer_(scalar_accessor::cloneBuffer(rhs))
  {}

  const_scalar_slice(const_scalar_slice&& rhs)
    : buffer_(std::move(rhs.buffer_))
  {}

  const auto operator[](size_t index) const {
    return const_scalar_slice<1>(scalar_buffer(1, buffer_, index));
  }

  template <unsigned M>
  const auto slice(size_t start = 0) const {
    return const_scalar_slice<M>(scalar_buffer(M, buffer_, start));
  }

protected:

  const scalar_buffer& get_buffer() const {
    return buffer_;
  }

  scalar_buffer& get_buffer() {
    return buffer_;
  }

  scalar_buffer buffer_;

  friend class scalar_accessor;
};

}
}

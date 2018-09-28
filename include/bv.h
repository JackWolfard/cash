#pragma once

#include "common.h"

namespace ch {
namespace internal {

template <typename T>
void bv_clear_extra_bits(T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  uint32_t extra_bits = size & WORD_MASK;
  if (extra_bits) {
    in[num_words-1] &= ~(WORD_MAX << extra_bits);
  }
}

template <typename U, typename T,
          CH_REQUIRE_0(std::is_integral_v<U> && std::is_unsigned_v<U>)>
void bv_assign(T* in, uint32_t size, U value) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;

  if constexpr (std::numeric_limits<U>::digits > 1) {
    CH_CHECK(log2ceil(value + 1) <= size, "value out of range");
  }

  if (size <= WORD_SIZE) {
    in[0] = value;
  } else {
    uint32_t num_words = ceildiv(size, WORD_SIZE);
    if (0 == value) {
      std::uninitialized_fill_n(in, num_words, 0);
      return;
    }

    if constexpr (bitwidth_v<U> <= WORD_SIZE) {
      in[0] = value;
      std::uninitialized_fill_n(in + 1, num_words - 1, 0);
    } else {
      auto len = std::min<uint32_t>(num_words, bitwidth_v<U> / WORD_SIZE);
      std::uninitialized_copy_n(reinterpret_cast<const T*>(&value), len, in);
      std::uninitialized_fill_n(in + len, num_words - len, 0);
    }
  }
}

template <typename U, typename T,
          CH_REQUIRE_0(std::is_integral_v<U> && std::is_signed_v<U>)>
void bv_assign(T* in, uint32_t size, U value) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  static_assert(std::numeric_limits<U>::digits > 1, "inavlid size");

  if (value >= 0) {
    bv_assign(in, size, std::make_unsigned_t<U>(value));
    return;
  }

  CH_CHECK(1 + log2ceil(~value + 1) <= size, "value out of range");

  if (size <= WORD_SIZE) {
    in[0] = value;
  } else {
    uint32_t num_words = ceildiv(size, WORD_SIZE);
    T ext_value  = ((value >= 0) ? 0 : WORD_MAX);
    if constexpr (bitwidth_v<U> <= WORD_SIZE) {
      in[0] = value;
      std::uninitialized_fill_n(in + 1, num_words - 1, ext_value);
    } else {
      auto len = std::min<uint32_t>(num_words, bitwidth_v<U> / WORD_SIZE);
      std::uninitialized_copy_n(reinterpret_cast<const T*>(&value), len, in);
      std::uninitialized_fill_n(in + len, num_words - len, ext_value);
    }
  }

  bv_clear_extra_bits(in, size);
}

template <typename T>
void bv_assign(T* in, uint32_t size, const std::string& value) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;

  int base = 0;
  int start = 0;
  size_t len = value.length();

  switch (value[len-1]) {
  case 'b':
    base = 2;
    break;
  case 'o':
    base = 8;
    break;
  case 'h':
    base = 16;
    if (len > 1 && (value[1] == 'x' || value[1] == 'X')) {
      start = 2;
    }
    break;
  default:
    CH_ABORT("invalid binary format, missing encoding base type.");
  }

  uint32_t log_base = log2ceil(base);
  len -= 2; // remove type info

  // calculate binary size
  uint32_t src_size = 0;
  for (uint32_t i = start; i < len; ++i) {
    char chr = value[i];
    if (chr == '\'')
      continue; // skip separator character
    if (0 == src_size) {
       // calculate exact bit coverage for the first non zero character
       int v = char2int(chr, base);
       if (v) {
         src_size += log2ceil(v+1);
       }
    } else {
      src_size += log_base;
    }
  }
  CH_CHECK(size >= src_size, "value out of range");

  // clear remaining words
  uint32_t num_words = ceildiv(size, WORD_SIZE);
  uint32_t src_num_words = ceildiv(src_size, WORD_SIZE);
  if (src_num_words < num_words) {
    std::uninitialized_fill_n(in + src_num_words, num_words - src_num_words, 0x0);
  }

  // write the value
  T w(0);
  T* dst = in;
  for (uint32_t i = 0, j = 0, n = len - start; i < n; ++i) {
    char chr = value[len - i - 1];
    if (chr == '\'')
      continue; // skip separator character
    T v = char2int(chr, base);
    w |= v << j;
    j += log_base;
    if (j >= WORD_SIZE) {
      *dst++ = w;
      j -= WORD_SIZE;
      w = v >> (log_base - j);
    }
  }

  if (w) {
    *dst = w;
  }
}

template <typename T>
bool bv_is_neg(const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;

  uint32_t index = (size - 1);
  uint32_t widx = index / WORD_SIZE;
  uint32_t wbit = index & WORD_MASK;
  auto mask = T(1) << wbit;
  return (in[widx] & mask) != 0;
}

template <typename T>
int bv_lsb(const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);
  assert(num_words);

  auto w = in[0];
  if (w) {
    return count_trailing_zeros<T>(w);
  }

  for (uint32_t i = 1; i < num_words; ++i) {
    auto w = in[i];
    if (w) {
      int z = count_trailing_zeros<T>(w);
      return z + i * WORD_SIZE;
    }
  }
  return -1;
}

template <typename T>
int bv_msb(const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);
  assert(num_words);

  int32_t i = num_words - 1;
  auto w = in[i];
  if (w) {
    int z = count_leading_zeros<T>(w);
    return (WORD_SIZE - z) + i * WORD_SIZE;
  }

  --i;
  for (; i >= 0; --i) {
    auto w = in[i];
    if (w) {
      int z = count_leading_zeros<T>(w);
      return (WORD_SIZE - z) + i * WORD_SIZE;
    }
  }
  return -1;
}

template <typename U, typename T>
T bv_cast(const T* in, uint32_t size) {
  static_assert(std::is_integral_v<T>, "invalid type");
  CH_CHECK(bitwidth_v<U> >= size, "invalid size");
  if constexpr (bitwidth_v<U> <= bitwidth_v<T>) {
    return bit_cast<U>(in[0]);
  } else {
    U ret(0);
    memcpy(&ret, in, ceildiv<uint32_t>(size, 8));
    return ret;
  }
}

template <typename T>
void bv_copy(T* dst, uint32_t dst_offset,
             const T* src, uint32_t src_offset,
             uint32_t length) {
  static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>, "invalid type");
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();

  uint32_t w_src_begin_idx = src_offset / WORD_SIZE;
  uint32_t w_src_begin_rem = src_offset & WORD_MASK;
  uint32_t src_end         = src_offset + length - 1;
  uint32_t w_src_end_idx   = (src_end / WORD_SIZE) - w_src_begin_idx;
  auto w_src               = src + w_src_begin_idx;

  uint32_t w_dst_begin_idx = dst_offset / WORD_SIZE;
  uint32_t w_dst_begin_rem = dst_offset & WORD_MASK;
  uint32_t dst_end         = dst_offset + length - 1;
  uint32_t w_dst_end_idx   = (dst_end / WORD_SIZE) - w_dst_begin_idx;
  uint32_t w_dst_end_rem   = dst_end & WORD_MASK;
  auto w_dst               = dst + w_dst_begin_idx;

  if (length <= WORD_SIZE) {
    T src_block = (w_src[0] >> w_src_begin_rem);
    if (1 == w_src_end_idx) {
      src_block |= (w_src[1] << (WORD_SIZE - w_src_begin_rem));
    }
    if (0 == w_dst_end_idx) {
      T mask = (WORD_MAX >> (WORD_SIZE - length)) << w_dst_begin_rem;
      w_dst[0] = blend<T>(mask, w_dst[0], (src_block << w_dst_begin_rem));
    } else {
      T mask = WORD_MAX << w_dst_begin_rem;
      w_dst[0] = blend<T>(mask, w_dst[0], (src_block << w_dst_begin_rem));
      T src_block_new = src_block >> (WORD_SIZE - w_dst_begin_rem);
      mask = ~((WORD_MAX << 1) << w_dst_end_rem);
      w_dst[1] = blend<T>(mask, w_dst[1], src_block_new);
    }
  } else
  if (0 == w_dst_begin_rem
   && 0 == w_src_begin_rem) {
    // update aligned blocks
    if (WORD_MASK == w_dst_end_rem) {
      std::uninitialized_copy_n(w_src, w_dst_end_idx + 1, w_dst);
    } else {
      std::uninitialized_copy_n(w_src, w_dst_end_idx, w_dst);
      // update remining block
      T mask = (WORD_MAX << 1) << w_dst_end_rem;
      w_dst[w_dst_end_idx] = blend<T>(mask, w_src[w_dst_end_idx], w_dst[w_dst_end_idx]);
    }
  } else
  if (0 == w_dst_begin_rem) {
    // update first block
    T src_block = *w_src++ >> w_src_begin_rem;
    src_block |= w_src[0] << (WORD_SIZE - w_src_begin_rem);
    w_dst[0] = src_block;

    // update intermediate blocks
    auto w_dst_end = (w_dst++) + w_dst_end_idx;
    while (w_dst < w_dst_end) {
      T src_block_new = (w_src[0] >> w_src_begin_rem) | (w_src[1] << (WORD_SIZE - w_src_begin_rem));
      ++w_src;
      *w_dst++ = src_block_new;
    }

    // update remining blocks
    T src_block_new = (w_src[0] >> w_src_begin_rem);
    if (w_src_end_idx > w_dst_end_idx) {
      src_block_new |= (w_src[1] << (WORD_SIZE - w_src_begin_rem));
    }
    T mask = ~((WORD_MAX << 1) << w_dst_end_rem);
    w_dst[0] = blend<T>(mask, w_dst[0], src_block_new);
  } else
  if (0 == w_src_begin_rem) {
    // update first block
    T src_block = *w_src++;
    T mask = WORD_MAX << w_dst_begin_rem;
    w_dst[0] = blend<T>(mask, w_dst[0], (src_block << w_dst_begin_rem));

    // update intermediate blocks
    auto w_dst_end = (w_dst++) + w_src_end_idx;
    while (w_dst < w_dst_end) {
      T src_block_new = *w_src++;
      *w_dst++ = (src_block_new << w_dst_begin_rem) | ((src_block >> 1) >> (WORD_MASK - w_dst_begin_rem));
      src_block = src_block_new;
    }

    // update remining blocks
    T src_block_new = (w_src[0] << w_dst_begin_rem) | ((src_block >> 1) >> (WORD_MASK - w_dst_begin_rem));
    if (w_src_end_idx < w_dst_end_idx) {
      *w_dst++ = src_block_new;
      src_block_new = (w_src[0] >> 1) >> (WORD_MASK - w_dst_begin_rem);
    }
    mask = ~((WORD_MAX << 1) << w_dst_end_rem);
    w_dst[0] = blend<T>(mask, w_dst[0], src_block_new);
  } else {
    // update first block
    T src_block = *w_src++ >> w_src_begin_rem;
    src_block |= w_src[0] << (WORD_SIZE - w_src_begin_rem);
    T mask = WORD_MAX << w_dst_begin_rem;
    w_dst[0] = blend<T>(mask, w_dst[0], (src_block << w_dst_begin_rem));

    // update intermediate blocks
    auto w_dst_end = (w_dst++) + std::min(w_src_end_idx, w_dst_end_idx);
    while (w_dst < w_dst_end) {
      T src_block_new = (w_src[0] >> w_src_begin_rem) | (w_src[1] << (WORD_SIZE - w_src_begin_rem));
      ++w_src;
      *w_dst++ = (src_block_new << w_dst_begin_rem) | ((src_block >> 1) >> (WORD_MASK - w_dst_begin_rem));
      src_block = src_block_new;
    }

    // update remining blocks
    T src_block_new;
    if (w_src_end_idx < w_dst_end_idx) {
      T tmp = w_src[0] >> w_src_begin_rem;
      src_block_new = (tmp << w_dst_begin_rem) | ((src_block >> 1) >> (WORD_MASK - w_dst_begin_rem));
      *w_dst++ = src_block_new;
      src_block_new = (tmp >> 1) >> (WORD_MASK - w_dst_begin_rem);
    } else
    if (w_src_end_idx == w_dst_end_idx) {
      src_block_new = ((w_src[0] >> w_src_begin_rem) << w_dst_begin_rem) | ((src_block >> 1) >> (WORD_MASK - w_dst_begin_rem));
    } else {
      src_block_new = ((w_src[0] >> w_src_begin_rem) | (w_src[1] << (WORD_SIZE - w_src_begin_rem))) << w_dst_begin_rem;
    }
    mask = ~((WORD_MAX << 1) << w_dst_end_rem);
    w_dst[0] = blend<T>(mask, w_dst[0], src_block_new);
  }
}

template <typename T>
bool bv_eq(const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (lhs[0] != rhs[0])
    return false;
  for (uint32_t i = 1; i < num_words; ++i) {
    if (lhs[i] != rhs[i])
      return false;
  }
  return true;
}

template <typename T>
bool bv_ult(const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (size <= WORD_SIZE) {
    return lhs[0] < rhs[0];
  }

  for (int32_t i = num_words - 1; i >= 0; --i) {
    if (lhs[i] < rhs[i])
      return true;
    if (lhs[i] > rhs[i])
      return false;
  }

  return false;
}

template <typename T>
bool bv_slt(const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (size <= WORD_SIZE) {
    auto u = sign_ext(lhs[0], size);
    auto v = sign_ext(rhs[0], size);
    return u < v;
  }

  // compare most signicant bits
  uint32_t msb_idx = size - 1;
  bool lhs_is_neg = lhs[msb_idx];
  bool rhs_is_neg = rhs[msb_idx];
  if (lhs_is_neg != rhs_is_neg)
    return lhs_is_neg;

  // same-sign words comparison
  for (int32_t i = num_words - 1; i >= 0; --i) {
    if (lhs[i] < rhs[i])
      return true;
    if (lhs[i] > rhs[i])
      return false;
  }

  return false;
}

template <typename T>
void bv_inv(T* out, const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  out[0] = ~in[0];
  for (uint32_t i = 1; i < num_words; ++i) {
    out[i] = ~in[i];
  }

  bv_clear_extra_bits(out, size);
}

template <typename T>
void bv_and(T* out, const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  out[0] = lhs[0] & rhs[0];
  for (uint32_t i = 1; i < num_words; ++i) {
    out[i] = lhs[i] & rhs[i];
  }
}

template <typename T>
void bv_or(T* out, const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  out[0] = lhs[0] | rhs[0];
  for (uint32_t i = 1; i < num_words; ++i) {
    out[i] = lhs[i] | rhs[i];
  }
}

template <typename T>
void bv_xor(T* out, const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  out[0] = lhs[0] ^ rhs[0];
  for (uint32_t i = 1; i < num_words; ++i) {
    out[i] = lhs[i] ^ rhs[i];
  }
}

template <typename T>
bool bv_andr(const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  auto rem = size & WORD_MASK;
  auto max = WORD_MAX >> (WORD_SIZE - rem);
  for (int32_t i = num_words - 1; i >= 0; --i) {
    if (in[i] != max)
      return false;
     max = WORD_MAX;
  }
  return true;
}

template <typename T>
bool bv_orr(const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (in[0])
    return true;
  for (uint32_t i = 1; i < num_words; ++i) {
    if (in[i])
      return true;
  }
  return false;
}

template <typename T>
bool bv_xorr(const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  bool ret = false;
  T tmp = in[0];
  for (uint32_t i = 1; i < num_words; ++i) {
    tmp ^= in[i];
  }
  for (uint32_t i = 0, n = std::min(size, WORD_SIZE); i < n; ++i) {
    ret ^= tmp & 0x1;
    tmp >>= 1;
  }
  return ret;
}

template <typename T>
void bv_sll(T* out, uint32_t out_size,
            const T* in, uint32_t in_size,
            uint32_t dist) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  uint32_t in_num_words = ceildiv(in_size, WORD_SIZE);
  uint32_t out_num_words = ceildiv(out_size, WORD_SIZE);

  if (1 == out_num_words) {
    out[0] = in[0] << dist;
  } else {
    uint32_t shift_words = std::min(dist / WORD_SIZE, out_num_words);
    std::uninitialized_fill_n(out, shift_words, 0x0);
    uint32_t shift_bits = dist & WORD_MASK;
    uint32_t shift_bits_r = WORD_SIZE - shift_bits;
    T carry(0), carry_mask(shift_bits ? WORD_MAX : 0);
    uint32_t count = out_num_words - shift_words;
    uint32_t i = 0;
    for (uint32_t n = std::min(in_num_words, count); i < n; ++i) {
      auto curr = in[i];
      out[i + shift_words] = (curr << shift_bits) | (carry & carry_mask);
      carry = curr >> shift_bits_r;
    }
    if ((i < count)) {
      out[(i++) + shift_words] = carry & carry_mask;
      std::uninitialized_fill_n(out + i + shift_words, count - i, 0x0);
    }
  }
  bv_clear_extra_bits(out, out_size);
}

template <typename T>
void bv_srl(T* out, uint32_t out_size,
            const T* in, uint32_t in_size,
            uint32_t dist) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  int32_t in_num_words = ceildiv(in_size, WORD_SIZE);
  int32_t out_num_words = ceildiv(out_size, WORD_SIZE);

  if (1 == in_num_words) {
    out[0] = in[0] >> dist;
  } else {
    int32_t shift_words = std::min<int32_t>(dist / WORD_SIZE, in_num_words);
    int32_t rem_words = in_num_words - shift_words;
    if (rem_words < out_num_words) {
      std::uninitialized_fill_n(out + rem_words, out_num_words - rem_words, 0x0);
    }
    int32_t shift_bits = dist & WORD_MASK;
    int32_t shift_bits_l = WORD_SIZE - shift_bits;
    T carry(0), carry_mask(shift_bits ? WORD_MAX : 0);
    int32_t i = in_num_words - 1 - shift_words;
    if (i >= out_num_words) {
      carry = in[out_num_words + shift_words] << shift_bits_l;
      i = out_num_words - 1;
    }
    for (; i >= 0; --i) {
      auto curr = in[i + shift_words];
      out[i] = (curr >> shift_bits) | (carry & carry_mask);
      carry = curr << shift_bits_l;
    }
  }
}

template <typename T>
void bv_sra(T* out, uint32_t out_size,
            const T* in, uint32_t in_size,
            uint32_t dist) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  int32_t in_num_words = ceildiv(in_size, WORD_SIZE);
  int32_t out_num_words = ceildiv(out_size, WORD_SIZE);

  T fill_value = bv_is_neg(in, in_size) ? WORD_MAX : 0;
  if (1 == in_num_words) {
    auto value = sign_ext(in[0], in_size);
    out[0] = value >> dist;
  } else {
    int32_t shift_words = std::min<int32_t>(dist / WORD_SIZE, in_num_words);
    int32_t rem_words = in_num_words - shift_words;
    if (rem_words < out_num_words) {
      std::uninitialized_fill_n(out + rem_words, out_num_words - rem_words, fill_value);
    }
    int32_t shift_bits = dist & WORD_MASK;
    int32_t shift_bits_l = WORD_SIZE - shift_bits;
    T carry(fill_value << shift_bits_l), carry_mask(shift_bits ? WORD_MAX : 0);
    int32_t i = in_num_words - 1 - shift_words;
    if (i >= out_num_words) {
      carry = in[out_num_words + shift_words] << shift_bits_l;
      i = out_num_words - 1;
    }
    for (; i >= 0; --i) {
      auto curr = in[i + shift_words];
      out[i] = (curr >> shift_bits) | (carry & carry_mask);
      carry = curr << shift_bits_l;
    }
  }
  if (fill_value) {
    bv_clear_extra_bits(out, out_size);
  }
}

template <typename T>
void bv_add(T* out, const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (size <= WORD_SIZE) {
    out[0] = lhs[0] + rhs[0];
  } else {
    T carry(0);
    for (uint32_t i = 0; i < num_words; ++i) {
      auto a = lhs[i];
      auto b = rhs[i];
      T c = a + b;
      T d = c + carry;
      carry = (c < a) || (d < carry);
      out[i] = d;
    }
  }
  bv_clear_extra_bits(out, size);
}

template <typename T>
void bv_sub(T* out, const T* lhs, const T* rhs, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (size <= WORD_SIZE) {
    out[0] = lhs[0] - rhs[0];
  } else {
    T borrow(0);
    for (uint32_t i = 0; i < num_words; ++i) {
      auto a = lhs[i];
      auto b = rhs[i];
      T c = a - b;
      T d = c - borrow;
      borrow = (a < c) || (c < d);
      out[i] = d;
    }
  }
  bv_clear_extra_bits(out, size);
}

template <typename T>
void bv_neg(T* out, const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (size <= WORD_SIZE) {
    out[0] = -in[0];
  } else {
    T borrow(0);
    for (uint32_t i = 0; i < num_words; ++i) {
      auto a = in[i];
      T b = -a - borrow;
      borrow = (a != 0) || (b != 0);
      out[i] = b;
    }
  }
  bv_clear_extra_bits(out, size);
}

template <typename T>
void bv_abs(T* out, const T* in, uint32_t size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  uint32_t num_words = ceildiv(size, WORD_SIZE);

  if (bv_is_neg(in, size)) {
    bv_neg(out, in, size);
  } else {
    std::uninitialized_copy_n(in, num_words, out);
  }
}

template <typename T>
void bv_mult(T* out, uint32_t out_size,
             const T* lhs, uint32_t lhs_size,
             const T* rhs, uint32_t rhs_size) {
  using xword_t = std::conditional_t<sizeof(T) == 1, uint8_t,
                    std::conditional_t<sizeof(T) == 2, uint16_t, uint32_t>>;
  using yword_t = std::conditional_t<sizeof(T) == 1, uint16_t,
                     std::conditional_t<sizeof(T) == 2, uint32_t, uint64_t>>;
  assert(out_size <= lhs_size + rhs_size);
  static constexpr uint32_t WORD_SIZE  = bitwidth_v<T>;
  static constexpr uint32_t XWORD_SIZE = bitwidth_v<xword_t>;

  if (out_size <= WORD_SIZE) {
    assert(lhs_size <= WORD_SIZE);
    assert(rhs_size <= WORD_SIZE);
    out[0] = lhs[0] * rhs[0];
  } else {
    auto m = ceildiv<int>(lhs_size, XWORD_SIZE);
    auto n = ceildiv<int>(rhs_size, XWORD_SIZE);
    auto p = ceildiv<int>(out_size, XWORD_SIZE);

    auto u = reinterpret_cast<const xword_t*>(lhs);
    auto v = reinterpret_cast<const xword_t*>(rhs);
    auto w = reinterpret_cast<xword_t*>(out);

    std::uninitialized_fill_n(w, p, 0);

    for (int i = 0; i < n; ++i) {
      xword_t tot(0);
      for (int j = 0, k = std::min(m, p - i); j < k; ++j) {
        auto c = yword_t(u[j]) * v[i] + w[i+j] + tot;
        tot = c >> XWORD_SIZE;
        w[i+j] = c;
      }
      if (i+m < p) {
        w[i+m] = tot;
      }
    }
  }

  bv_clear_extra_bits(out, out_size);
}

template <typename T>
void bv_udiv(T* quot, uint32_t quot_size,
             T* rem, uint32_t rem_size,
             const T* lhs, uint32_t lhs_size,
             const T* rhs, uint32_t rhs_size) {
  assert(lhs_size && rhs_size);
  using xword_t = std::conditional_t<sizeof(T) == 1, uint8_t,
                    std::conditional_t<sizeof(T) == 2, uint16_t, uint32_t>>;
  using yword_t = std::conditional_t<sizeof(T) == 1, uint16_t,
                     std::conditional_t<sizeof(T) == 2, uint32_t, uint64_t>>;
  using syword_t = std::make_signed_t<yword_t>;

  static constexpr uint32_t XWORD_SIZE = bitwidth_v<xword_t>;
  static constexpr xword_t  XWORD_MAX  = std::numeric_limits<xword_t>::max();

  auto m  = ceildiv<int>(bv_msb(lhs, lhs_size), XWORD_SIZE);
  auto n  = ceildiv<int>(bv_msb(rhs, rhs_size), XWORD_SIZE);
  auto qn = ceildiv<int>(quot_size, XWORD_SIZE);
  auto rn = ceildiv<int>(rem_size, XWORD_SIZE);

  auto u = reinterpret_cast<const xword_t*>(lhs);
  auto v = reinterpret_cast<const xword_t*>(rhs);
  auto q = reinterpret_cast<xword_t*>(quot);
  auto r = reinterpret_cast<xword_t*>(rem);

  CH_CHECK(n >= 1, "invalid size");

  // reset the outputs
  if (qn) {
    std::uninitialized_fill_n(q, qn, 0);
  }

  if (rn) {
    std::uninitialized_fill_n(r, rn, 0);
  }

  // early exit
  if (m <= 0 || m < n || u[m] < v[n]) {
    if (rn) {
      for (int i = 0; i < std::min(m, rn); ++i) {
        r[i] = u[i];
      }
    }
    return;
  }

  std::vector<xword_t> tu(2 * (m + 1), 0), tv(2 * n, 0);
  auto un = tu.data();
  auto vn = tv.data();

  // normalize
  int s = count_leading_zeros<xword_t>(v[n - 1]);
  un[m] = u[m - 1] >> (XWORD_SIZE - s);
  for (int i = m - 1; i > 0; --i) {
    un[i] = (u[i] << s) | (u[i - 1] >> (XWORD_SIZE - s));
  }
  un[0] = u[0] << s;
  for (int i = n - 1; i > 0; --i) {
    vn[i] = (v[i] << s) | (v[i - 1] >> (XWORD_SIZE - s));
  }
  vn[0] = v[0] << s;

  auto h = vn[n - 1];

  for (int j = m - n; j >= 0; --j) {
    // estimate quotient
    auto w = (yword_t(un[j + n]) << XWORD_SIZE) | un[j + n - 1];
    auto qhat = w / h;

    // multiply and subtract
    xword_t k(0);
    for (int i = 0; i < n; ++i) {
      auto p = qhat * vn[i];
      auto w = un[i + j] - k - (p & XWORD_MAX);
      k = (p >> XWORD_SIZE) - (w >> XWORD_SIZE);
      un[i + j] = w;
    }

    syword_t t(un[j + n] - k);
    un[j + n] = t;

    if (j < qn)
      q[j] = qhat;

    // overflow handling
    if (t < 0) {
      if (j < qn)
        --q[j];
      yword_t k(0);
      for (int i = 0; i < n; ++i) {
        auto w = un[i + j] + k + vn[i];
        k = (w >> XWORD_SIZE);
        un[i + j] = w;
      }
      un[j + n] += k;
    }
  }

  // unnormalize remainder
  if (rn) {
    for (int i = 0; i < std::min(n, rn); ++i) {
      r[i] = (un[i] >> s) | (un[i + 1] << (XWORD_SIZE - s));
    }
  }
}

template <typename T>
void bv_udiv(T* out, uint32_t out_size, const T* lhs, uint32_t lhs_size,
             const T* rhs, uint32_t rhs_size) {
  static constexpr uint32_t WORD_SIZE  = bitwidth_v<T>;
  assert(lhs_size && rhs_size && out_size);

  if (lhs_size <= WORD_SIZE
   && rhs_size <= WORD_SIZE
   && out_size <= WORD_SIZE) {
    out[0] = lhs[0] / rhs[0];
  } else {
    T* r = nullptr;
    bv_udiv(out, out_size, r, 0, lhs, lhs_size, rhs, rhs_size);
  }
}

template <typename T>
void bv_sdiv(T* out, uint32_t out_size,
             const T* lhs, uint32_t lhs_size,
             const T* rhs, uint32_t rhs_size) {
  static constexpr uint32_t WORD_SIZE  = bitwidth_v<T>;
  assert(lhs_size && rhs_size && out_size);

  if (lhs_size <= WORD_SIZE
   && rhs_size <= WORD_SIZE
   && out_size <= WORD_SIZE) {
    auto u = sign_ext(lhs[0], lhs_size);
    auto v = sign_ext(rhs[0], rhs_size);
    out[0] = u / v;
    bv_clear_extra_bits(out, out_size);
  } else {
    std::vector<T> u(ceildiv<uint32_t>(lhs_size, WORD_SIZE));
    std::vector<T> v(ceildiv<uint32_t>(rhs_size, WORD_SIZE));
    bv_abs(u.data(), lhs, lhs_size);
    bv_abs(v.data(), rhs, rhs_size);
    T* r = nullptr;
    bv_udiv(out, out_size, r, 0, u.data(), lhs_size, v.data(), rhs_size);
    if (bv_is_neg(lhs, lhs_size) ^ bv_is_neg(rhs, rhs_size)) {
      bv_neg(out, out, out_size);
    }
  }
}

template <typename T>
void bv_umod(T* out, uint32_t out_size,
             const T* lhs, uint32_t lhs_size,
             const T* rhs, uint32_t rhs_size) {
  static constexpr uint32_t WORD_SIZE  = bitwidth_v<T>;
  assert(lhs_size && rhs_size && out_size);

  if (lhs_size <= WORD_SIZE
   && rhs_size <= WORD_SIZE
   && out_size <= WORD_SIZE) {
    out[0] = lhs[0] % rhs[0];
  } else {
    T* q = nullptr;
    bv_udiv(q, 0, out, out_size, lhs, lhs_size, rhs, rhs_size);
  }
}

template <typename T>
void bv_smod(T* out, uint32_t out_size,
             const T* lhs, uint32_t lhs_size,
             const T* rhs, uint32_t rhs_size) {
  static constexpr uint32_t WORD_SIZE  = bitwidth_v<T>;
  assert(lhs_size && rhs_size && out_size);

  if (lhs_size <= WORD_SIZE
   && rhs_size <= WORD_SIZE
   && out_size <= WORD_SIZE) {
    auto u = sign_ext(lhs[0], lhs_size);
    auto v = sign_ext(rhs[0], rhs_size);
    out[0] = u % v;
    bv_clear_extra_bits(out, out_size);
  } else {
    std::vector<T> u(ceildiv<uint32_t>(lhs_size, WORD_SIZE));
    std::vector<T> v(ceildiv<uint32_t>(rhs_size, WORD_SIZE));
    bv_abs(u.data(), lhs, lhs_size);
    bv_abs(v.data(), rhs, rhs_size);
    T* q = nullptr;
    bv_udiv(q, 0, out, out_size, u.data(), lhs_size, v.data(), rhs_size);
    if (bv_is_neg(lhs, lhs_size)) {
      bv_neg(out, out, out_size);
    }
  }
}

template <typename T>
void bv_zext(T* out, uint32_t out_size, const T* in, uint32_t in_size) {
  static constexpr uint32_t WORD_SIZE  = bitwidth_v<T>;
  assert(in_size <= out_size);

  uint32_t in_num_words = ceildiv(in_size, WORD_SIZE);
  uint32_t out_num_words = ceildiv(out_size, WORD_SIZE);

  if (out_size <= WORD_SIZE) {
    out[0] = in[0];
    return;
  }

  std::uninitialized_copy_n(in, in_num_words, out);
  std::uninitialized_fill(out + in_num_words, out + out_num_words, 0);
}

template <typename T>
void bv_sext(T* out, uint32_t out_size, const T* in, uint32_t in_size) {
  static constexpr uint32_t WORD_SIZE = bitwidth_v<T>;
  static constexpr uint32_t WORD_MASK = WORD_SIZE - 1;
  static constexpr T        WORD_MAX  = std::numeric_limits<T>::max();
  assert(in_size <= out_size);

  uint32_t in_num_words = ceildiv(in_size, WORD_SIZE);
  uint32_t out_num_words = ceildiv(out_size, WORD_SIZE);
  auto is_neg = bv_is_neg(in, in_size);

  if (out_size <= WORD_SIZE) {
    if (is_neg) {
      out[0] = sign_ext(in[0], in_size);
      bv_clear_extra_bits(out, out_size);
    } else {
      out[0] = in[0];
    }
    return;
  }

  if (is_neg) {
    uint32_t msb = in_size - 1;
    uint32_t msb_blk_idx = msb / WORD_SIZE;
    uint32_t msb_blk_rem = msb & WORD_MASK;
    auto msb_blk = in[msb_blk_idx];
    std::uninitialized_copy_n(in, in_num_words, out);
    out[msb_blk_idx] = msb_blk | (WORD_MAX << msb_blk_rem);
    std::uninitialized_fill(out + in_num_words, out + out_num_words, WORD_MAX);
    bv_clear_extra_bits(out, out_size);
  } else {
    std::uninitialized_copy_n(in, in_num_words, out);
    std::uninitialized_fill(out + in_num_words, out + out_num_words, 0);
  }
}

}
}

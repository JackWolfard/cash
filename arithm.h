#pragma once

#include "bitv.h"

#define CHDL_BINOP_GEN0(func, type) \
  template <unsigned N> ch_bitv<N> func(const ch_bitbase<N>& a, type b) { return func(a, ch_bitv<N>(b)); } \
  template <unsigned N> ch_bitv<N> func(type a, const ch_bitbase<N>& b) { return func(ch_bitv<N>(a), b); }
  
#define CHDL_BINOP_GEN1(func) \
  template <unsigned N, unsigned M> ch_bitv<CHDL_MAX(N,M)> func(const ch_bitbase<N>& a, const ch_bitbase<M>& b) { return func(ch_zext<CHDL_MAX(N,M)>(a), ch_zext<CHDL_MAX(N,M)>(b)); } \
  CHDL_BINOP_GEN0(func, char) \
  CHDL_BINOP_GEN0(func, int8_t) \
  CHDL_BINOP_GEN0(func, uint8_t) \
  CHDL_BINOP_GEN0(func, int16_t) \
  CHDL_BINOP_GEN0(func, uint16_t) \
  CHDL_BINOP_GEN0(func, int32_t) \
  CHDL_BINOP_GEN0(func, uint32_t) \
  CHDL_BINOP_GEN0(func, int64_t) \
  CHDL_BINOP_GEN0(func, uint64_t)
    
  
#define CHDL_BINOP_GEN2(func, op) \
  CHDL_BINOP_GEN1(func) \
  template <unsigned N, unsigned M> ch_bitv<CHDL_MAX(N,M)> op(const ch_bitbase<N>& a, const ch_bitbase<M>& b) { return func(a, b); } \
  CHDL_BINOP_GEN0(op, char) \
  CHDL_BINOP_GEN0(op, int8_t) \
  CHDL_BINOP_GEN0(op, uint8_t) \
  CHDL_BINOP_GEN0(op, int16_t) \
  CHDL_BINOP_GEN0(op, uint16_t) \
  CHDL_BINOP_GEN0(op, int32_t) \
  CHDL_BINOP_GEN0(op, uint32_t) \
  CHDL_BINOP_GEN0(op, int64_t) \
  CHDL_BINOP_GEN0(op, uint64_t)

#define CHDL_UNARYOP_GEN(func, op) \
  template <unsigned N> ch_bitv<N> op(const ch_bitbase<N>& in) { return func(in); }

#define CHDL_COMPAREOP_GEN0(func, op, type) \
  template <unsigned N> ch_logic func(const ch_bitbase<N>& a, type b) { return func(a, ch_bitv<N>(b)); } \
  template <unsigned N> ch_logic func(type a, const ch_bitbase<N>& b) { return func(ch_bitv<N>(a), b); } \
  template <unsigned N> ch_logic op(const ch_bitbase<N>& a, type b) { return func(a, ch_bitv<N>(b)); } \
  template <unsigned N> ch_logic op(type a, const ch_bitbase<N>& b) { return func(ch_bitv<N>(a), b); }
  
#define CHDL_COMPAREOP_GEN1(func, op) \
  template <unsigned N> ch_logic op(const ch_bitbase<N>& a, const ch_bitbase<N>& b) { return func(a, b); } \
  CHDL_COMPAREOP_GEN0(func, op, char) \
  CHDL_COMPAREOP_GEN0(func, op, int8_t) \
  CHDL_COMPAREOP_GEN0(func, op, uint8_t) \
  CHDL_COMPAREOP_GEN0(func, op, int16_t) \
  CHDL_COMPAREOP_GEN0(func, op, uint16_t) \
  CHDL_COMPAREOP_GEN0(func, op, int32_t) \
  CHDL_COMPAREOP_GEN0(func, op, uint32_t) \
  CHDL_COMPAREOP_GEN0(func, op, int64_t) \
  CHDL_COMPAREOP_GEN0(func, op, uint64_t)

#define CHDL_SHIFTOP_GEN0(func, type) \
  template <unsigned N> ch_bitv<N> func(const ch_bitbase<N>& a, type b) { return func(a, ch_bitv<32>(b)); }
  
#define CHDL_SHIFTOP_GEN1(func) \
  CHDL_SHIFTOP_GEN0(func, char) \
  CHDL_SHIFTOP_GEN0(func, int8_t) \
  CHDL_SHIFTOP_GEN0(func, uint8_t) \
  CHDL_SHIFTOP_GEN0(func, int16_t) \
  CHDL_SHIFTOP_GEN0(func, uint16_t) \
  CHDL_SHIFTOP_GEN0(func, int32_t) \
  CHDL_SHIFTOP_GEN0(func, uint32_t) \
  CHDL_SHIFTOP_GEN0(func, int64_t) \
  CHDL_SHIFTOP_GEN0(func, uint64_t)

#define CHDL_SHIFTOP_GEN2(func, op) \
  CHDL_SHIFTOP_GEN1(func) \
  template <unsigned N, unsigned M> ch_bitv<N> op(const ch_bitbase<N>& a, const ch_bitbase<M>& b) { return func(a, b); } \
  CHDL_SHIFTOP_GEN0(op, char) \
  CHDL_SHIFTOP_GEN0(op, int8_t) \
  CHDL_SHIFTOP_GEN0(op, uint8_t) \
  CHDL_SHIFTOP_GEN0(op, int16_t) \
  CHDL_SHIFTOP_GEN0(op, uint16_t) \
  CHDL_SHIFTOP_GEN0(op, int32_t) \
  CHDL_SHIFTOP_GEN0(op, uint32_t) \
  CHDL_SHIFTOP_GEN0(op, int64_t) \
  CHDL_SHIFTOP_GEN0(op, uint64_t)

namespace chdl_internal {

enum ch_operator {
  op_inv,
  op_and,
  op_or,
  op_xor,
  op_nand,
  op_nor,
  op_xnor,
  op_andr,
  op_orr,
  op_xorr,
  op_sll,
  op_slr,
  op_rotl,
  op_rotr,
  op_add,
  op_sub,
  op_neg,
  op_mult,
  op_div,
  op_mod,
  op_eq,
  op_ne,
  op_lt,
  op_gt,
  op_le,
  op_ge,
};

lnode createAluNode(ch_operator op, uint32_t size, const lnode& a, const lnode& b);
lnode createAluNode(ch_operator op, uint32_t size, const lnode& a);

// compare operators

CHDL_COMPAREOP_GEN1(ch_eq, operator==)
CHDL_COMPAREOP_GEN1(ch_ne, operator!=)
CHDL_COMPAREOP_GEN1(ch_lt, operator<)
CHDL_COMPAREOP_GEN1(ch_gt, operator>)
CHDL_COMPAREOP_GEN1(ch_le, operator<=)
CHDL_COMPAREOP_GEN1(ch_ge, operator>=)

// logic operators

CHDL_BINOP_GEN2(ch_and, operator&)
CHDL_BINOP_GEN2(ch_or, operator|)
CHDL_BINOP_GEN2(ch_xor, operator^)
CHDL_UNARYOP_GEN(ch_inv, operator~)
CHDL_BINOP_GEN1(ch_nand)
CHDL_BINOP_GEN1(ch_nor)
CHDL_BINOP_GEN1(ch_xnor)

// arithmetic operators

CHDL_UNARYOP_GEN(ch_neg, operator-)
CHDL_BINOP_GEN2(ch_add, operator+)
CHDL_BINOP_GEN2(ch_sub, operator-)
CHDL_BINOP_GEN2(ch_mult, operator*)
CHDL_BINOP_GEN2(ch_div, operator/)
CHDL_BINOP_GEN2(ch_mod, operator%)

// shift operators

CHDL_SHIFTOP_GEN2(ch_sll, operator<<)
CHDL_SHIFTOP_GEN2(ch_slr, operator>>)
CHDL_SHIFTOP_GEN1(ch_rotl)
CHDL_SHIFTOP_GEN1(ch_rotr)

///////////////////////////////////////////////////////////////////////////////

template <ch_operator op, unsigned N, unsigned M>
ch_bitv<N> OpBinary(const ch_bitbase<N>& a, const ch_bitbase<M>& b) {
  return ch_bitv<N>(createAluNode(op, N, a, b));
}

template <ch_operator op, unsigned N>
ch_logic OpCompare(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return ch_logic(createAluNode(op, 1, a, b));
}

template <ch_operator op, unsigned N>
ch_bitv<N> OpUnary(const ch_bitbase<N>& a) {
  return ch_bitv<N>(createAluNode(op, N, a));
}

template <ch_operator op, unsigned N>
ch_logic OpReduce(const ch_bitbase<N>& a) {
  return ch_logic(createAluNode(op, 1, a));
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N>
ch_bitv<N> ch_inv(const ch_bitbase<N>& a) {
  return OpUnary<op_inv>(a);
}

template <unsigned N>
ch_bitv<N> ch_nand(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_nand>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_nor(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_nor>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_and(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_and>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_or(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_or>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_xor(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_xor>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_xnor(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_xnor>(a, b);
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N> 
ch_logic ch_andr(const ch_bitbase<N>& a) {
  return OpReduce<op_andr>(a);
}

template <unsigned N> 
ch_logic ch_orr(const ch_bitbase<N>& a) {
  return OpReduce<op_orr>(a);
}

template <unsigned N> 
ch_logic ch_xorr(const ch_bitbase<N>& a) {
  return OpReduce<op_xorr>(a);
}

///////////////////////////////////////////////////////////////////////////////

inline ch_logic operator! (const ch_logicbase& a) {
  return ch_inv(a);         
}

inline ch_logic operator&& (const ch_logicbase& a, const ch_logicbase& b) {
  return ch_and(a, b);      
}

inline ch_logic operator|| (const ch_logicbase& a, const ch_logicbase& b) {
  return ch_or(a, b);      
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N> 
ch_logic ch_eq(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpCompare<op_eq>(a, b);
}

template <unsigned N> 
ch_logic ch_ne(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpCompare<op_ne>(a, b);
}

template <unsigned N>
ch_logic ch_lt(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpCompare<op_lt>(a, b);
}

template <unsigned N>
ch_logic ch_gt(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpCompare<op_gt>(a, b);
}

template <unsigned N>
ch_logic ch_ge(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpCompare<op_ge>(a, b);
}

template <unsigned N>
ch_logic ch_le(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpCompare<op_le>(a, b);
}  

///////////////////////////////////////////////////////////////////////////////

template <unsigned N, unsigned M>
ch_bitv<N> ch_sll(const ch_bitbase<N>& a, const ch_bitbase<M>& b) {
  return OpBinary<op_sll>(a, b);
}

template <unsigned N, unsigned M>
ch_bitv<N> ch_slr(const ch_bitbase<N>& a, const ch_bitbase<M>& b) {
  return OpBinary<op_slr>(a, b);
}  

template <unsigned N, unsigned M>
ch_bitv<N> ch_rotl(const ch_bitbase<N>& a, const ch_bitbase<M>& b) {
  return OpBinary<op_rotl>(a, b);
}

template <unsigned N, unsigned M>
ch_bitv<N> ch_rotr(const ch_bitbase<N>& a, const ch_bitbase<M>& b) {
  return OpBinary<op_rotr>(a, b);
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N>
ch_bitv<N> ch_add(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_add>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_sub(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_sub>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_neg(const ch_bitbase<N>& a) {
  return OpUnary<op_neg>(a);
}

template <unsigned N>
ch_bitv<N> ch_mult(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_mult>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_div(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_div>(a, b);
}

template <unsigned N>
ch_bitv<N> ch_mod(const ch_bitbase<N>& a, const ch_bitbase<N>& b) {
  return OpBinary<op_mod>(a, b);
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N, unsigned S>
ch_bitv<(N >> S)> ch_mux(const ch_bitbase<N>& in, const ch_bitbase<S>& sel) {
  TODO("Not yet implemented!");
}

template <unsigned N, unsigned S> 
ch_bitv<(N << S)> ch_demux(const ch_bitbase<N>& in, const ch_bitbase<S>& sel) {
  TODO("Not yet implemented!");
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N> 
ch_bitv<CLOG2(N)> ch_log2(const ch_bitbase<N>& a) {
  TODO("Not yet implemented!");
}

template <unsigned N> 
ch_bitv<CLOG2(N)> ch_lsb(const ch_bitbase<N>& a) {
  TODO("Not yet implemented!");
}

template <unsigned N> 
ch_bitv<CLOG2(N)> ch_enc(const ch_bitbase<N>& a) {
  TODO("Not yet implemented!");
}

template <unsigned N> 
ch_bitv<(1 << N)> ch_dec(const ch_bitbase<N>& a) {
  TODO("Not yet implemented!");
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N>
void ch_print(const ch_bitbase<N>& a) {
  TODO("Not yet implemented!");
}

}

#undef CHDL_BINOP_GEN0
#undef CHDL_BINOP_GEN1
#undef CHDL_BINOP_GEN2
#undef CHDL_UNARYOP_GEN  
#undef CHDL_COMPAREOP_GEN0
#undef CHDL_COMPAREOP_GEN1
#undef CHDL_SHIFTOP_GEN0
#undef CHDL_SHIFTOP_GEN1
#undef CHDL_SHIFTOP_GEN2

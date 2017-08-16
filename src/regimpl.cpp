#include "regimpl.h"
#include "context.h"
#include "reg.h"
#include "select.h"

using namespace std;
using namespace cash::detail;

regimpl::regimpl(const lnode& next)
  : lnodeimpl(op_reg, next.get_ctx(), next.get_size()) {
  context* ctx = next.get_ctx();
  const lnode& clk = ctx->get_clk();

  cd_ = ctx->create_cdomain({clock_event(clk, EDGE_POS)});
  cd_->add_use(this);

  srcs_.emplace_back(next);
  srcs_.emplace_back(clk);
}

regimpl::~regimpl() {
  cd_->remove_use(this);
}

void regimpl::tick(ch_cycle t) { 
  CH_UNUSED(t);
  value_ = q_next_;
}

void regimpl::tick_next(ch_cycle t) {
  q_next_ = srcs_[0].eval(t);
}

const bitvector& regimpl::eval(ch_cycle t) {
  CH_UNUSED(t);
  return value_; 
}

void regimpl::print_vl(ostream& out) const {
  CH_UNUSED(out);
  CH_TODO();
}

///////////////////////////////////////////////////////////////////////////////

latchimpl::latchimpl(const lnode& next,
                     const lnode& init,
                     const lnode& enable,
                     const lnode& reset)
  : lnodeimpl(op_latch, next.get_ctx(), next.get_size()) {
  context* ctx = next.get_ctx();

  cd_ = ctx->create_cdomain(
    {clock_event(enable, EDGE_ANY), clock_event(next, EDGE_ANY),
     clock_event(reset, EDGE_ANY), clock_event(init, EDGE_ANY)});
  cd_->add_use(this);

  srcs_.emplace_back(next);
  srcs_.emplace_back(init);
  srcs_.emplace_back(enable);
  srcs_.emplace_back(reset);  
}

latchimpl::~latchimpl() {
  cd_->remove_use(this);
}

void latchimpl::tick(ch_cycle t) { 
  CH_UNUSED(t);
}

void latchimpl::tick_next(ch_cycle t) {
  if (srcs_[3].eval(t)[0]) {
    value_ = srcs_[1].eval(t);
  } else if (srcs_[2].eval(t)[0]) {
    value_ = srcs_[0].eval(t);
  }
}

const bitvector& latchimpl::eval(ch_cycle t) {
  CH_UNUSED(t);
  return value_; 
}

void latchimpl::print_vl(ostream& out) const {
  CH_UNUSED(out);
  CH_TODO();
}

///////////////////////////////////////////////////////////////////////////////

ch_bit<1> cash::detail::ch_clock() {
  return ch_bit<1>(ctx_curr()->get_clk());
}

void cash::detail::ch_pushclock(const ch_bitbase<1>& clk) {
  ctx_curr()->push_clk(get_node(clk));
}

void cash::detail::ch_popclock() {
  ctx_curr()->pop_clk();
}

ch_bit<1> cash::detail::ch_reset() {
  return ch_bit<1>(ctx_curr()->get_reset());
}

void cash::detail::ch_pushreset(const ch_bitbase<1>& reset) {
  ctx_curr()->push_reset(get_node(reset));
}

void cash::detail::ch_popreset() {
  ctx_curr()->pop_reset();
}

lnodeimpl* cash::detail::createRegNode(const lnode& next, const lnode& init) {
  context* const ctx = ctx_curr();
  if (nullptr == init)
    init = ctx->create_literal(bitvector(next->get_size(), 0));
  return new regimpl(createSelectNode(ctx->get_reset(), init, next));
}

lnodeimpl* cash::detail::createLatchNode(
    const lnode& next,
    const lnode& init,
    const lnode& enable,
    const lnode& reset) {
  return new latchimpl(next, init, enable, reset);
}

lnodeimpl* cash::detail::createReadyNode(const lnode& node) {
  CH_UNUSED(node);
  CH_TODO();
}

lnodeimpl* cash::detail::createValidNode(const lnode& node) {
  CH_UNUSED(node);
  CH_TODO();
}

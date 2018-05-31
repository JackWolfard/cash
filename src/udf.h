#pragma once

#include "logic.h"

namespace ch {
namespace internal {

class udf_iface : public refcounted {
public:
  udf_iface(uint32_t delta,
            bool enable,
            uint32_t output_size,
            const std::initializer_list<uint32_t>& inputs_size)
    : delta_(delta)
    , enable_(enable)
    , output_size_(output_size)
    , inputs_size_(inputs_size)
  {}

  virtual ~udf_iface() {}

  uint32_t delta() const {
    return delta_;
  }

  bool enable() const {
    return enable_;
  }

  uint32_t output_size() const {
    return output_size_;
  }

  const std::vector<uint32_t>& inputs_size() const {
    return inputs_size_;
  }

  virtual void initialize() = 0;

  virtual void reset() = 0;

  virtual void eval(bitvector& out, const std::vector<lnode>& srcs) = 0;

  virtual void init_verilog(std::ostream& out) = 0;

  virtual void to_verilog(std::ostream& out) = 0;

private:

  uint32_t delta_;
  bool enable_;
  uint32_t output_size_;
  std::vector<uint32_t> inputs_size_;
};

template <uint32_t Delta, bool HasEnable, typename Output_, typename... Inputs_>
struct udf_traits {
  static constexpr uint32_t delta = Delta;
  static constexpr bool has_enable = HasEnable;
  using Output = Output_;
  using Inputs = std::tuple<Inputs_...>;
};

template <typename Output, typename... Inputs>
class udf_comb : public udf_iface {
public:
  using traits = udf_traits<0, false, Output, Inputs...>;

  udf_comb() : udf_iface(0, false, width_v<Output>, {width_v<Inputs>...}) {}

  void initialize() override {}

  void reset() override {}

  void init_verilog(std::ostream&) override {}

  void to_verilog(std::ostream&) override {}
};

template <uint32_t Delta, bool HasEnable, typename Output, typename... Inputs>
class udf_seq : public udf_iface {
public:
  static_assert(Delta != 0, "invalid delta value");
  using traits = udf_traits<Delta, HasEnable, Output, Inputs...>;

  udf_seq() : udf_iface(Delta, HasEnable, width_v<Output>, {width_v<Inputs>...}) {}

  void initialize() override {}

  void reset() override {}

  void init_verilog(std::ostream&) override {}

  void to_verilog(std::ostream&) override {}
};

udf_iface* lookupUDF(const std::type_index& signature);

void registerUDF(const std::type_index& signature, udf_iface* udf);

lnodeimpl* create_udf_node(udf_iface* udf, const std::initializer_list<lnode>& inputs);

template <typename T>
udf_iface* get_udf() {
  auto signature = std::type_index(typeid(T));
  auto udf = lookupUDF(signature);
  if (nullptr == udf) {
    udf = new T();
    registerUDF(signature, udf);
  }
  return udf;
}

template <typename T, typename... Args>
auto ch_udf(Args&& ...args) {
  static_assert(sizeof...(Args) >= std::tuple_size_v<typename T::traits::Inputs>, "number of inputs mismatch");
  auto node = create_udf_node(get_udf<T>(), {get_lnode(args)...});
  return typename T::traits::Output(make_logic_buffer(node));
}

}}

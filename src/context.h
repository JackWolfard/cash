#pragma once

#include "common.h"

namespace ch {
namespace internal {

class bitvector;
class lnode;
class nodelist;
class lnodeimpl;
class undefimpl;
class proxyimpl;
class aluimpl;
class litimpl;
class ioimpl;
class inputimpl;
class outputimpl;
class bindimpl;
class selectimpl;
class tapimpl;
class assertimpl;
class timeimpl;
class cdimpl;
class clock_event;

using ch_tick = uint64_t;

typedef std::unordered_map<uint32_t, std::vector<const lnode*>> node_map_t;

typedef ordered_set<lnodeimpl*> live_nodes_t;

struct cond_block_t;

struct cond_range_t {
  uint32_t offset;
  uint32_t length;

  bool operator==(const cond_range_t& var) const {
    return this->offset == var.offset
        && this->length == var.length;
  }

  bool operator!=(const cond_range_t& var) const {
    return !(*this == var);
  }

  bool operator<(const cond_range_t& var) const {
    if (this->offset != var.offset)
      return (this->offset < var.offset);
    return (this->length < var.length);
  }
};

typedef std::unordered_map<uint32_t, lnodeimpl*> cond_defs_t;

typedef std::map<cond_range_t, cond_defs_t> cond_slices_t;

typedef std::unordered_map<lnodeimpl*, cond_slices_t> cond_vars_t;

typedef std::list<lnodeimpl*> node_list_t;

struct cond_block_t;

struct cond_br_t {
  cond_br_t(lnodeimpl* p_key, cond_block_t* p_parent, const source_location& p_sloc)
    : key(p_key)
    , parent(p_parent)
    , sloc(p_sloc)
  {}

  ~cond_br_t();

  lnodeimpl* key;
  cond_block_t* parent;
  const source_location sloc;
  std::list<cond_block_t*> blocks;
};

struct cond_block_t {
  cond_block_t(uint32_t p_id, lnodeimpl* p_pred, cond_br_t* p_branch)
    : id(p_id)
    , pred(p_pred)
    , branch(p_branch)
  {}

  uint32_t id;
  lnodeimpl* pred;
  cond_br_t* branch;
  std::list<cond_br_t*> branches;
};

inline cond_br_t::~cond_br_t() {
  for (auto block : blocks) {
    delete block;
  }
}

typedef std::unordered_map<uint32_t, cond_block_t*> cond_inits_t;

struct alu_key_t {
  uint32_t op;
  uint32_t arg0;
  uint32_t arg1;

  bool operator==(const alu_key_t& rhs) const {
    return this->op   == rhs.op
        && this->arg0 == rhs.arg0
        && this->arg1 == rhs.arg1;
  }
};

struct hash_alu_key_t {
  std::size_t operator()(const alu_key_t& key) const {
    return key.op ^ key.arg0 ^ key.arg1;
  }
};

typedef std::unordered_map<alu_key_t, aluimpl*, hash_alu_key_t> alu_cache_t;

typedef const char* (*enum_string_cb)(uint32_t value);

typedef std::unordered_map<uint32_t, enum_string_cb> enum_strings_t;

class context : public refcounted {
public:

  uint32_t get_id() const {
    return id_;
  }

  const std::string& get_name() const {
    return name_;
  }

  const auto& get_nodes() const {
    return nodes_;
  }

  auto& get_nodes() {
    return nodes_;
  }

  const auto& get_undefs() const {
    return undefs_;
  }

  const auto& get_proxies() const {
    return proxies_;
  }

  const auto& get_inputs() const {
    return inputs_;
  }

  const auto& get_outputs() const {
    return outputs_;
  }

  const auto& get_taps() const {
    return taps_;
  }

  const auto& get_gtaps() const {
    return gtaps_;
  }

  const auto& get_literals() const {
    return literals_;
  }

  auto get_default_clk() const {
    return default_clk_;
  }

  auto get_default_reset() const {
    return default_reset_;
  }

  const auto& get_bindings() const {
    return bindings_;
  }

  const auto& get_cdomains() const {
    return cdomains_;
  }

  //--

  void push_cd(cdimpl* cd);

  void pop_cd();

  cdimpl* current_cd();

  //--

  lnodeimpl* get_time();

  //--

  uint32_t node_id();

  template <typename T, typename... Ts>
  T* create_node(Ts&&... args) {
    auto node = new T(this, std::forward<Ts>(args)...);
    this->add_node(node);
    return node;
  }

  cdimpl* create_cdomain(const lnode& clock, const lnode& reset, bool posedge);

  aluimpl* create_alu(uint32_t op, const lnode& in);

  aluimpl* create_alu(uint32_t op, const lnode& lhs, const lnode& rhs);

  node_list_t::iterator destroyNode(const node_list_t::iterator& it);
  
  //--

  void begin_branch(lnodeimpl* key, const source_location& sloc);

  void end_branch();

  void begin_block(lnodeimpl* pred);

  void end_block();

  bool conditional_enabled(lnodeimpl* node = nullptr) const;

  void conditional_assign(lnodeimpl* dst,
                          uint32_t offset,
                          uint32_t length,
                          lnodeimpl* src);

  lnodeimpl* get_predicate();

  void remove_local_variable(lnodeimpl* src, lnodeimpl* dst);

  //--
  
  lnodeimpl* get_literal(const bitvector& value);

  //--

  void register_tap(const std::string& name, const lnode& lnode);

  //--
  
  void syntax_check();
    
  //--
  
  live_nodes_t compute_live_nodes() const;
  
  //--
  
  void tick(ch_tick t);

  void tick_next(ch_tick t);

  void eval(ch_tick t);

  //--
  
  void dump_ast(std::ostream& out, uint32_t level);

  void dump_cfg(lnodeimpl* node, std::ostream& out, uint32_t level);  

  void dump_stats(std::ostream& out);

  //--

  void bind_input(const lnode& src, const lnode& input);

  void bind_output(const lnode& dst, const lnode& output);

  //--

  void register_enum_string(const lnode& node, enum_string_cb callback);

  const char* enum_to_string(const lnode& node, ch_tick t);
  
protected:

  context(const std::string& name);

  ~context();

  void add_node(lnodeimpl* node);

  node_list_t::iterator remove_node(const node_list_t::iterator& it);

  lnodeimpl* emit_conditionals(lnodeimpl* dst,
                               const cond_range_t& range,
                               const cond_defs_t& defs,
                               const cond_br_t* branch);

  bindimpl* get_binding(context* module);

  uint32_t    id_;
  std::string name_;

  uint32_t    node_ids_;
  uint32_t    block_idx_;
  inputimpl*  default_clk_;
  inputimpl*  default_reset_;
  timeimpl*   time_;
  
  node_list_t             nodes_;
  std::list<undefimpl*>   undefs_;
  std::list<proxyimpl*>   proxies_;
  std::list<inputimpl*>   inputs_;
  std::list<outputimpl*>  outputs_;
  std::list<tapimpl*>     taps_;
  std::list<ioimpl*>      gtaps_;
  std::list<litimpl*>     literals_;
  std::list<cdimpl*>      cdomains_;
  std::list<bindimpl*>    bindings_;

  std::stack<cond_br_t*>  cond_branches_;
  cond_vars_t             cond_vars_;
  cond_inits_t            cond_inits_;

  std::stack<cdimpl*>     cd_stack_;

  alu_cache_t             alu_cache_;

  unique_name             unique_tap_names_;

  enum_strings_t          enum_strings_;

  friend class context_manager;
};

context* ctx_create(size_t signature, const std::string& name);

context* ctx_swap(context* ctx);

context* ctx_curr();

}
}

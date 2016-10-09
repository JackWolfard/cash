#pragma once

#include "lnodeimpl.h"
#include "tickable.h"

namespace chdl_internal {

enum EDGE_DIR {
  EDGE_POS = 0,
  EDGE_NEG = 1,
  EDGE_ANY = 2,
};

class clock_event {
public:  
  clock_event(const lnode& signal, EDGE_DIR edgedir);
  ~clock_event();
  
  const lnode& get_signal() const {
    return m_signal;
  }
  
  EDGE_DIR get_edgedir() const {
    return m_edgedir;
  }
  
  bool operator==(const lnode& n) const {
    return (m_signal.get_id() == n.get_id());
  }
  
  bool operator==(const clock_event& e) const {
    return (m_signal.get_id() == e.get_signal().get_id()) && 
           (m_edgedir == e.get_edgedir());
  }
  
  bool eval(ch_cycle t);
  
  void print_vl(std::ostream& out) const;
  
protected:
  lnode    m_signal;
  EDGE_DIR m_edgedir;
  bool     m_cval; 
};

///////////////////////////////////////////////////////////////////////////////

class cdomain : public refcounted {
public:
  cdomain(context_ptr ctx, const std::vector<clock_event>& sensitivity_list);
  
  void add_use(tickable* reg);
  void remove_use(tickable* reg);
  
  const std::vector<clock_event>& get_sensitivity_list() const {
    return m_sensitivity_list;
  }
  
  context_ptr get_ctx() const {
    return m_ctx;
  }
  
  void tick(ch_cycle t);
  void tick_next(ch_cycle t);
  
  bool operator==(const std::vector<clock_event>& events) const;
  
  void print_vl(std::ostream& out) const;

protected:
  
  ~cdomain();
  
  std::vector<clock_event> m_sensitivity_list;
  std::list<tickable*> m_regs;
  context_ptr m_ctx;
};

typedef refcounted_ptr<cdomain> cdomain_ptr;

}

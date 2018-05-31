#include "memimpl.h"
#include "mem.h"
#include "context.h"
#include "proxyimpl.h"

using namespace ch::internal;

std::vector<uint8_t> toByteVector(const std::string& init_file,
                                  uint32_t data_width,
                                  uint32_t num_items) {
  std::vector<uint8_t> packed(CH_CEILDIV(data_width * num_items, 8));
  std::ifstream in(init_file, std::ios::binary);
  in.read((char*)packed.data(), packed.size());
  return packed;
}

///////////////////////////////////////////////////////////////////////////////

memimpl::memimpl(context* ctx,
                 uint32_t data_width,
                 uint32_t num_items,
                 bool write_enable,
                 const std::vector<uint8_t>& init_data)
  : ioimpl(ctx, type_mem, data_width * num_items)
  , data_width_(data_width)
  , num_items_(num_items)
  , write_enable_(write_enable)
  , has_initdata_(!init_data.empty()) {
  if (write_enable) {
    auto cd = ctx->current_cd();
    cd->add_reg(this);
    cd->acquire();
    srcs_.emplace_back(cd);
  }
  if (has_initdata_) {
    assert(8 * init_data.size() >= value_.size());
    value_.write(0, init_data.data(), init_data.size(), 0, value_.size());
  }
}

memimpl::~memimpl() {
  if (write_enable_) {
    auto cd = reinterpret_cast<cdimpl*>(this->cd().impl());
    cd->remove_reg(this);
    cd->release();
  }
}

void memimpl::remove_port(memportimpl* port) {
  for (auto it = ports_.begin(), end = ports_.end(); it != end; ++it) {
    if ((*it)->id() == port->id()) {
      ports_.erase(it);
      break;
    }
  }
}

memportimpl* memimpl::port(const lnode& addr) {
  for (auto port : ports_) {
    if (port->addr().id() == addr.id()) {
      return port;
    }
  }
  auto impl = ctx_->create_node<memportimpl>(this, ports_.size(), addr);
  ports_.emplace_back(impl);
  return ports_.back();
}

void memimpl::tick() {
  for (auto port : ports_) {
    port->tick();
  }
}

void memimpl::eval() {
  //--
}

void memimpl::print(std::ostream& out, uint32_t level) const {
  CH_UNUSED(level);
  out << "#" << id_ << " <- " << this->type() << value_.size();
  uint32_t n = srcs_.size();
  if (n > 0) {
    out << "(";
    for (uint32_t i = 0; i < n; ++i) {
      if (i > 0)
        out << ", ";
      out << "#" << srcs_[i].id();
    }
    out << ")";
  }
  if (level == 2) {
    out << " = " << value_;
  }
}

///////////////////////////////////////////////////////////////////////////////

memportimpl::memportimpl(context* ctx, memimpl* mem, uint32_t index, const lnode& addr)
  : ioimpl(ctx, type_memport, mem->data_width())
  , index_(index)
  , read_enable_(false)
  , a_next_(0)  
  , wdata_idx_(-1)
  , wenable_idx_(-1)
  , dirty_(false) {
  mem->acquire();
  srcs_.emplace_back(mem);
  srcs_.emplace_back(addr);
}

memportimpl::~memportimpl() {
  auto mem = dynamic_cast<memimpl*>(this->mem().impl());
  mem->remove_port(this);
  mem->release();
}


void memportimpl::read() {
  read_enable_ = true;
}

void memportimpl::write(const lnode& data) {
  if (wdata_idx_ == -1) {
    // add write port to memory sources to enforce DFG dependencies
    auto mem = dynamic_cast<memimpl*>(srcs_[0].impl());
    mem->srcs().emplace_back(this);
  }
  // add data source
  wdata_idx_ = this->add_src(wdata_idx_, data);
}

void memportimpl::write(const lnode& data, const lnode& enable) {
  // add data source
  this->write(data);

  // add enable predicate
  wenable_idx_ = this->add_src(wenable_idx_, enable);
}

void memportimpl::tick() {
  if (dirty_) {
    auto mem = dynamic_cast<memimpl*>(srcs_[0].impl());
    auto data_width = mem->data_width();
    mem->value().write(a_next_ * data_width,
                       q_next_.words(),
                       q_next_.cbsize(),
                       0,
                       data_width);
  }
}

void memportimpl::eval() {
  // asynchronous read
  auto mem = dynamic_cast<memimpl*>(srcs_[0].impl());
  auto data_width = mem->data_width();
  uint32_t addr = srcs_[1].data().word(0);
  mem->value().read(0,
                    value_.words(),
                    value_.cbsize(),
                    addr * data_width,
                    data_width);

  // synchronous memory write
  if (wdata_idx_ != -1) {
    dirty_ = (wenable_idx_ != -1) ? srcs_[wenable_idx_].data().word(0) : true;
    if (dirty_) {
      a_next_ = addr;
      q_next_ = srcs_[wdata_idx_].data();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

memory::memory(uint32_t data_width,
               uint32_t num_items,
               bool write_enable,
               const std::vector<uint8_t>& init_data) {
  CH_CHECK(!ctx_curr()->conditional_enabled(), "memory objects cannot be nested inside a conditional block");
  impl_ = ctx_curr()->create_node<memimpl>(data_width, num_items, write_enable, init_data);
}

lnodeimpl* memory::read(const lnode& addr) const {
  auto port = impl_->port(addr);
  port->read();
  return port;
}

void memory::write(const lnode& addr, const lnode& value) {
  auto port = impl_->port(addr);
  port->write(value);
}

void memory::write(const lnode& addr, const lnode& value, const lnode& enable) {
  auto port = impl_->port(addr);
  port->write(value, enable);
}

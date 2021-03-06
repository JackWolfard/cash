#include "ioport.h"
#include "ioimpl.h"
#include "moduleimpl.h"
#include "proxyimpl.h"
#include "context.h"

using namespace ch::internal;

ioimpl::ioimpl(context* ctx,
               lnodetype type,
               uint32_t size,
               const std::string& name,
               const source_location& sloc)
  : lnodeimpl(ctx->node_id(), type, size, ctx, name, sloc)
{}

///////////////////////////////////////////////////////////////////////////////

ioportimpl::ioportimpl(context* ctx,
                       lnodetype type,
                       uint32_t size,
                       const io_value_t& value,
                       const std::string& name,
                       const source_location& sloc)
  : ioimpl(ctx, type, size, name, sloc)
  , value_(value)
{}

///////////////////////////////////////////////////////////////////////////////

inputimpl::inputimpl(context* ctx,
                     uint32_t size,
                     const io_value_t& value,
                     const std::string& name,
                     const source_location& sloc)
  : ioportimpl(ctx, type_input, size, value, name, sloc)
{}

inputimpl::~inputimpl() {}

lnodeimpl* inputimpl::clone(context* ctx, const clone_map&) const {
  return ctx->create_node<inputimpl>(this->size(), value_, name_, sloc_);
}

void inputimpl::print(std::ostream& out) const {
  out << "#" << id_ << " <- " << this->type() << this->size();
  out << "(" << name_;
  for (auto port : bindings_) {
    out << ", $" << port->id();
  }
  out << ")";
}

///////////////////////////////////////////////////////////////////////////////

outputimpl::outputimpl(context* ctx,
                       uint32_t size,
                       lnodeimpl* src,
                       const io_value_t& value,
                       const std::string& name,
                       const source_location& sloc)
  : ioportimpl(ctx, type_output, size, value, name, sloc) {
  this->add_src(src);
}

outputimpl::~outputimpl() {}

lnodeimpl* outputimpl::clone(context* ctx, const clone_map& cloned_nodes) const {
  auto src = cloned_nodes.at(this->src(0).id());
  return ctx->create_node<outputimpl>(this->size(), src, value_, name_, sloc_);
}

void outputimpl::print(std::ostream& out) const {
  out << "#" << id_ << " <- " << this->type() << this->size();
  out << "(" << name_ << ", #" << this->src(0).id() << ")";
}

///////////////////////////////////////////////////////////////////////////////

tapimpl::tapimpl(context* ctx, 
                 lnodeimpl* target, 
                 const std::string& name,
                 const source_location& sloc)
  : ioportimpl(ctx,
               type_tap,
               target->size(),
               smart_ptr<sdata_type>::make(target->size()),
               name,
               sloc) {
  this->add_src(target);
}

tapimpl::~tapimpl() {}

lnodeimpl* tapimpl::clone(context* ctx, const clone_map& cloned_nodes) const {
  auto target = cloned_nodes.at(this->target().id());
  return ctx->create_node<tapimpl>(target, name_, sloc_);
}

void tapimpl::print(std::ostream& out) const {
  out << "#" << id_ << " <- " << this->type() << this->size();
  out << "(" << name_ << ", #" << this->target().id() << ")";
}

///////////////////////////////////////////////////////////////////////////////

bypassimpl::bypassimpl(context* ctx, lnodeimpl* target)
  : ioimpl(ctx,
           type_bypass,
           target->size(),
           target->name(),
           target->sloc())
  , target_(target) {
}

bypassimpl::~bypassimpl() {}

lnodeimpl* bypassimpl::clone(context*, const clone_map&) const {
  assert(false);
  return nullptr;
}

void bypassimpl::print(std::ostream& out) const {
  out << "#" << id_ << " <- " << this->type() << this->size();
  out << "(" << name_ << ", #" << target_->id() << ")";
}

///////////////////////////////////////////////////////////////////////////////

lnodeimpl* ch::internal::createInputNode(uint32_t size,
                                         const std::string& name, 
                                         const source_location& sloc) {
  return ctx_curr()->create_input(size, name, sloc);
}

lnodeimpl* ch::internal::createOutputNode(uint32_t size,
                                          const std::string& name,
                                          const source_location& sloc) {
  auto output = ctx_curr()->create_output(size, name, sloc);
  return output->src(0).impl();
}

lnodeimpl* ch::internal::getOutputNode(const std::string& name) {
  return ctx_curr()->get_output(name);
}

///////////////////////////////////////////////////////////////////////////////

system_io_buffer::system_io_buffer(uint32_t size, const std::string& name)
  : base(0, name) {
  size_ = size;
}

system_io_buffer::system_io_buffer(const lnode& io)
  : base(0, io.name())
  , io_(reinterpret_cast<ioportimpl*>(io.impl())->value()) {
  size_ = io.size();
}

const sdata_type& system_io_buffer::data() const {
  return *io_;
}

void system_io_buffer::read(uint32_t src_offset,
                            sdata_type& dst,
                            uint32_t dst_offset,
                            uint32_t length) const {
  return dst.copy(dst_offset, *io_, src_offset, length);
}

void system_io_buffer::write(uint32_t dst_offset,
                             const sdata_type& src,
                             uint32_t src_offset,
                             uint32_t length) {
  io_->copy(dst_offset, src, src_offset, length);
}

void system_io_buffer::read(uint32_t src_offset,
                            void* out,
                            uint32_t byte_alignment,
                            uint32_t dst_offset,
                            uint32_t length) const {
  return io_->read(src_offset, out, byte_alignment, dst_offset, length);
}

void system_io_buffer::write(uint32_t dst_offset,
                             const void* in,
                             uint32_t byte_alignment,
                             uint32_t src_offset,
                             uint32_t length) {
  io_->write(dst_offset, in, byte_alignment, src_offset, length);
}

void system_io_buffer::bind(const io_value_t& io) {
  io_.reset_all(io);
}

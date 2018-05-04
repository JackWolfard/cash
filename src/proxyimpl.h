#pragma once

#include "lnodeimpl.h"

namespace ch {
namespace internal {

class proxyimpl : public lnodeimpl {
public:

  struct range_t {
    uint32_t src_idx;
    uint32_t dst_offset;
    uint32_t src_offset;
    uint32_t length;

    bool operator==(const range_t& rhs) const {
      return this->src_idx    == rhs.src_idx
          && this->dst_offset == rhs.dst_offset
          && this->src_offset == rhs.src_offset
          && this->length     == rhs.length;
    }

    bool operator!=(const range_t& rhs) const {
      return !(*this == rhs);
    }
  };

  bool is_identity() const {
    return (1 == ranges_.size())
        && (srcs_[0].size() == value_.size());
  }

  const std::vector<range_t>& ranges() const {
    return ranges_;
  }
  
  void add_source(uint32_t dst_offset,
                  const lnode& src,
                  uint32_t src_offset = 0,
                  uint32_t length = 0);

  std::vector<lnode>::iterator erase_source(std::vector<lnode>::iterator iter);

  bool equals(const lnodeimpl& rhs) const override;

  std::size_t hash() const override;

  lnodeimpl* slice(uint32_t offset, uint32_t length) override;

  const bitvector& eval(ch_tick t) override;

  void print(std::ostream& out, uint32_t level) const override;
  
protected:

  proxyimpl(context* ctx,
            uint32_t size,
            unsigned var_id = 0,
            const std::string& name = "",
            const source_location& sloc = source_location());

  proxyimpl(context* ctx,
            const lnode& src,
            uint32_t offset,
            uint32_t length,
            unsigned var_id = 0,
            const std::string& name = "",
            const source_location& sloc = source_location());

  proxyimpl(context* ctx, const lnode& src, uint32_t offset = 0)
    : proxyimpl(ctx, src, offset, src.size())
  {}

  ~proxyimpl() {}

  std::vector<range_t> ranges_; 
  ch_tick tick_;

  friend class context;
};

}
}

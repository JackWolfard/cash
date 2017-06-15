#pragma once

#include "lnodeimpl.h"

namespace cash {
namespace detail {

class wireimpl : public lnodeimpl {
public:
  wireimpl(lnodeimpl* src);

  const lnode& get_src() const {
    return srcs_[0];
  }

  lnode& get_src() {
    return srcs_[0];
  }
};

}
}

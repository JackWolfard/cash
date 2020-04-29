#pragma once

#include "../core.h"

namespace ch
{
namespace htl
{

using namespace ch::logic;

template <typename T>
class ch_elastic
{
private:
  class __system_io__;
  class __system_flip_io__;
  class __logic_struct__;
  class __system_struct__;

  class __logic_flip_io__
  {
  public:
    using traits = ch::internal::logic_io_traits<(
        ch_width_v<ch::internal::identity_t<ch_in<T>>>
        + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>
        + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
      ), 
      ch_direction(
        ch_flip_io<ch::internal::identity_t<ch_in<T>>>::traits::direction
        | ch_flip_io<ch::internal::identity_t<ch_in<ch_bool>>>::traits::direction 
        | ch_flip_io<ch::internal::identity_t<ch_out<ch_bool>>>::traits::direction
      ), 
      __logic_flip_io__, ch_elastic, __system_flip_io__, __system_io__,
      __system_struct__, __logic_struct__
    >;

    ch_flip_io<ch::internal::identity_t<ch_in<T>>> data;
    ch_flip_io<ch::internal::identity_t<ch_in<ch_bool>>> valid;
    ch_flip_io<ch::internal::identity_t<ch_out<ch_bool>>> ready;

    explicit __logic_flip_io__(
      const std::string &name = "io", 
      const ch::internal::source_location &sloc = ch::internal::source_location(
        "include/htl/elastic.h", 15, 0
      )
    ) : data(ch::internal::stringf("%s.%s", name.c_str(), "data"), sloc),
        valid(ch::internal::stringf("%s.%s", name.c_str(), "valid"), sloc),
        ready(ch::internal::stringf("%s.%s", name.c_str(), "ready"), sloc) {}

    explicit __logic_flip_io__(
      const typename traits::flip_io &__other,
      const ch::internal::source_location &sloc = ch::internal::source_location(
        "include/htl/elastic.h", 15, 0
      )
    ) : data(__other.data, sloc), valid(__other.valid, sloc),
        ready(__other.ready, sloc) {}

    explicit __logic_flip_io__(
      const typename traits::system_flip_io &__other,
      const ch::internal::source_location &sloc = ch::internal::source_location(
        "include/htl/elastic.h", 15, 0
      )
    ) : data(__other.data, sloc), valid(__other.valid, sloc),
        ready(__other.ready, sloc) {}

    __logic_flip_io__(
      const __logic_flip_io__ &__other
    ) : data(__other.data), valid(__other.valid), ready(__other.ready) {}

    __logic_flip_io__(
      __logic_flip_io__ &&__other
    ) : data(std::move(__other.data)), valid(std::move(__other.valid)),
        ready(std::move(__other.ready)) {}

    void operator()(typename traits::flip_io &__other)
    {
      this->data(__other.data);
      this->valid(__other.valid);
      this->ready(__other.ready);
    }

  protected:
    __logic_flip_io__ &operator=(const __logic_flip_io__ &) = delete;
    __logic_flip_io__ &operator=(__logic_flip_io__ &&) = delete;
    friend ch_ostream &operator<<(ch_ostream &__out, const __logic_flip_io__ &__in)
    {
      __out << "(";
      if (0)
      {
        __out << ", ";
      }
      __out << "ready"
            << "=" << __in.ready;
      if (1)
      {
        __out << ", ";
      }
      __out << "valid"
            << "=" << __in.valid;
      if (2)
      {
        __out << ", ";
      }
      __out << "data"
            << "=" << __in.data;
      __out << ")";
      return __out;
    }
  };

  class __system_io__
  {
  public:
    using traits = ch::internal::system_io_traits<(
        ch_width_v<ch::internal::identity_t<ch_in<T>>>
        + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>
        + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
      ),
      ch_direction(
        ch::internal::identity_t<ch_in<T>>::traits::direction
        | ch::internal::identity_t<ch_in<ch_bool>>::traits::direction
        | ch::internal::identity_t<ch_out<ch_bool>>::traits::direction
      ),
      __system_io__, __system_flip_io__, ch_elastic, __logic_flip_io__,
      __system_struct__, __logic_struct__
    >;

    ch_system_io<ch::internal::identity_t<ch_in<T>>> data;
    ch_system_io<ch::internal::identity_t<ch_in<ch_bool>>> valid;
    ch_system_io<ch::internal::identity_t<ch_out<ch_bool>>> ready;

    explicit __system_io__(
      const std::string &name = "io"
    ) : data(ch::internal::stringf("%s.%s", name.c_str(), "data")),
        valid(ch::internal::stringf("%s.%s", name.c_str(), "valid")),
        ready(ch::internal::stringf("%s.%s", name.c_str(), "ready")) {}

    explicit __system_io__(
      const typename traits::logic_flip_io &__other
    ) : data(__other.data), valid(__other.valid), ready(__other.ready) {}
    
    __system_io__(
      const __system_io__ &__other
    ) : data(__other.data), valid(__other.valid), ready(__other.ready) {}

    __system_io__(
      __system_io__ &&__other
    ) : data(std::move(__other.data)), valid(std::move(__other.valid)), 
        ready(std::move(__other.ready)) {}

    void operator()(typename traits::flip_io &__other)
    {
      this->data(__other.data);
      this->valid(__other.valid);
      this->ready(__other.ready);
    }

  protected:
    __system_io__ &operator=(const __system_io__ &) = delete;
    __system_io__ &operator=(__system_io__ &&) = delete;
    friend std::ostream &operator<<(std::ostream &__out, const __system_io__ &__in)
    {
      __out << "(";
      if (0)
      {
        __out << ", ";
      }
      __out << "ready"
            << "=" << __in.ready;
      if (1)
      {
        __out << ", ";
      }
      __out << "valid"
            << "=" << __in.valid;
      if (2)
      {
        __out << ", ";
      }
      __out << "data"
            << "=" << __in.data;
      __out << ")";
      return __out;
    }
  };

  class __system_flip_io__
  {
  public:
    using traits = ch::internal::system_io_traits<(
        ch_width_v<ch::internal::identity_t<ch_in<T>>>
        + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>
        + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
      ),
      ch_direction(
        ch::internal::identity_t<ch_in<T>>::traits::direction 
        | ch::internal::identity_t<ch_in<ch_bool>>::traits::direction
        | ch::internal::identity_t<ch_out<ch_bool>>::traits::direction
      ),
      __system_flip_io__, __system_io__, __logic_flip_io__, ch_elastic,
      __system_struct__, __logic_struct__
    >;

    ch_system_io<ch_flip_io<ch::internal::identity_t<ch_in<T>>>> data;
    ch_system_io<ch_flip_io<ch::internal::identity_t<ch_in<ch_bool>>>> valid;
    ch_system_io<ch_flip_io<ch::internal::identity_t<ch_out<ch_bool>>>> ready;

    explicit __system_flip_io__(
      const std::string &name = "io"
    ) : data(ch::internal::stringf("%s.%s", name.c_str(), "data")),
        valid(ch::internal::stringf("%s.%s", name.c_str(), "valid")),
        ready(ch::internal::stringf("%s.%s", name.c_str(), "ready")) {}

    explicit __system_flip_io__(
      const typename traits::logic_flip_io &__other
    ) : data(__other.data), valid(__other.valid), ready(__other.ready) {}

    __system_flip_io__(
      const __system_flip_io__ &__other
    ) : data(__other.data), valid(__other.valid), ready(__other.ready) {}

    __system_flip_io__(
      __system_flip_io__ &&__other
    ) : data(std::move(__other.data)), valid(std::move(__other.valid)),
        ready(std::move(__other.ready)) {}

    void operator()(typename traits::flip_io &__other)
    {
      this->data(__other.data);
      this->valid(__other.valid);
      this->ready(__other.ready);
    }

  protected:
    __system_flip_io__ &operator=(const __system_flip_io__ &) = delete;
    __system_flip_io__ &operator=(__system_flip_io__ &&) = delete;
    friend std::ostream &operator<<(std::ostream &__out, const __system_flip_io__ &__in)
    {
      __out << "(";
      if (0)
      {
        __out << ", ";
      }
      __out << "ready"
            << "=" << __in.ready;
      if (1)
      {
        __out << ", ";
      }
      __out << "valid"
            << "=" << __in.valid;
      if (2)
      {
        __out << ", ";
      }
      __out << "data"
            << "=" << __in.data;
      __out << ")";
      return __out;
    }
  };
  
  class __system_struct__
  {
  public:
    using traits = ch::internal::system_traits<(
        ch_width_v<ch::internal::identity_t<ch_in<T>>>
        + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>
        + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
      ),
      false, __system_struct__, __logic_struct__
    >;

  private:
    enum
    {
      __field_offset0 = 0,
      __field_offset1 = __field_offset0 + ch_width_v<ch::internal::identity_t<ch_in<T>>>,
      __field_offset2 = __field_offset1 + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>,
      __field_offset3 = __field_offset2 + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
    };

  public:
    ch_system_t<ch::internal::identity_t<ch_in<T>>> data;
    ch_system_t<ch::internal::identity_t<ch_in<ch_bool>>> valid;
    ch_system_t<ch::internal::identity_t<ch_out<ch_bool>>> ready;

    explicit __system_struct__(
      const ch::internal::system_buffer &buffer = ch::internal::make_system_buffer(traits::bitwidth)
    ) : data(ch::internal::make_system_buffer(
          ch_width_v<ch::internal::identity_t<ch_in<T>>>, buffer, __field_offset0
        )),
        valid(ch::internal::make_system_buffer(
          ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>, buffer, __field_offset1
        )), 
        ready(ch::internal::make_system_buffer(
          ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>, buffer, __field_offset2
        )) {}

    __system_struct__(
      const ch_system_t<ch::internal::identity_t<ch_out<ch_bool>>> &_ready,
      const ch_system_t<ch::internal::identity_t<ch_in<ch_bool>>> &_valid,
      const ch_system_t<ch::internal::identity_t<ch_in<T>>> &_data
    ) : __system_struct__(ch::internal::make_system_buffer(traits::bitwidth))
    {
      ready = _ready;
      valid = _valid;
      data = _data;
    }

    __system_struct__(
      const __system_struct__ &__other
    ) : __system_struct__(ch::internal::make_system_buffer(traits::bitwidth)) {
      this->operator=(__other);
    }

    __system_struct__(
      __system_struct__ &&__other
    ) : __system_struct__(ch::internal::system_accessor::move(__other)) {}

    __system_struct__ &operator=(const __system_struct__ &__other)
    {
      ch::internal::system_accessor::assign(*this, __other);
      return *this;
    }

    __system_struct__ &operator=(__system_struct__ &&__other)
    {
      ch::internal::system_accessor::move(*this, std::move(__other));
      return *this;
    }

  protected:
    const ch::internal::system_buffer &__buffer() const {
      return ch::internal::system_accessor::source(data); 
    }
    friend std::ostream &operator<<(std::ostream &__out, const __system_struct__ &__in)
    {
      __out << "(";
      if (0)
      {
        __out << ", ";
      }
      __out << "ready"
            << "=" << __in.ready;
      if (1)
      {
        __out << ", ";
      }
      __out << "valid"
            << "=" << __in.valid;
      if (2)
      {
        __out << ", ";
      }
      __out << "data"
            << "=" << __in.data;
      __out << ")";
      return __out;
    }
    friend class ch::internal::system_accessor;

  public:
    template <typename __R>
    auto as() const
    {
      _Static_assert(ch::internal::is_system_type_v<__R>, "invalid type");
      auto self = reinterpret_cast<const __system_struct__ *>(this);
      return ch::internal::system_accessor::cast<std::add_const_t<__R>>(*self);
    }
    template <typename __R>
    auto as()
    {
      _Static_assert(ch::internal::is_system_type_v<__R>, "invalid type");
      auto self = reinterpret_cast<const __system_struct__ *>(this);
      return ch::internal::system_accessor::cast<__R>(*self);
    }
    auto as_bit() const {
      return this->template as<ch_sbit<__system_struct__::traits::bitwidth>>();
    }
    auto as_bit() {
      return this->template as<ch_sbit<__system_struct__::traits::bitwidth>>();
    }
    auto as_int() const {
      return this->template as<ch_sint<__system_struct__::traits::bitwidth>>();
    }
    auto as_int() {
      return this->template as<ch_sint<__system_struct__::traits::bitwidth>>();
    }
    auto as_uint() const { 
      return this->template as<ch_suint<__system_struct__::traits::bitwidth>>();
    }
    auto as_uint() {
      return this->template as<ch_suint<__system_struct__::traits::bitwidth>>();
    }
    auto ref()
    {
      auto self = reinterpret_cast<const __system_struct__ *>(this);
      return ch::internal::system_accessor::sliceref<__system_struct__>(*self, 0);
    }
  };

  class __logic_struct__
  {
  public:
    using traits = ch::internal::logic_traits<(
        ch_width_v<ch::internal::identity_t<ch_in<T>>>
        + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>
        + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
      ),
      false, __logic_struct__, __system_struct__
    >;

  private:
    enum
    {
      __field_offset0 = 0,
      __field_offset1 = __field_offset0 + ch_width_v<ch::internal::identity_t<ch_in<T>>>,
      __field_offset2 = __field_offset1 + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>,
      __field_offset3 = __field_offset2 + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
    };

  public:
    ch_logic_t<ch::internal::identity_t<ch_in<T>>> data;
    ch_logic_t<ch::internal::identity_t<ch_in<ch_bool>>> valid;
    ch_logic_t<ch::internal::identity_t<ch_out<ch_bool>>> ready;

    explicit __logic_struct__(
      const ch::internal::logic_buffer &buffer = ch::internal::make_logic_buffer(
        traits::bitwidth,
        ch::internal::source_info(
          ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
        )
      )) : data(ch::internal::make_logic_buffer(
            ch_width_v<ch::internal::identity_t<ch_in<T>>>, buffer, __field_offset0,
            ch::internal::source_info(buffer.sloc(), "data")
          )),
          valid(ch::internal::make_logic_buffer(
            ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>, buffer,
            __field_offset1, ch::internal::source_info(buffer.sloc(), "valid")
          )),
          ready(ch::internal::make_logic_buffer(
            ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>, buffer,
            __field_offset2, ch::internal::source_info(buffer.sloc(), "ready")
          )) {}

    __logic_struct__(
      const ch_logic_t<ch::internal::identity_t<ch_out<ch_bool>>> &_ready,
      const ch_logic_t<ch::internal::identity_t<ch_in<ch_bool>>> &_valid,
      const ch_logic_t<ch::internal::identity_t<ch_in<T>>> &_data,
      const ch::internal::source_info &srcinfo = ch::internal::source_info(
        ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
      )
    ) : __logic_struct__(
          ch::internal::make_logic_buffer(traits::bitwidth, srcinfo)
        )
    {
      ready = _ready;
      valid = _valid;
      data = _data;
    }

    __logic_struct__(
      const __logic_struct__ &__other,
      const ch::internal::source_info &srcinfo = ch::internal::source_info(
        ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
      )
    ) : __logic_struct__(ch::internal::make_logic_buffer(
          traits::bitwidth, srcinfo
        )) {
      this->operator=(__other);
    }

    __logic_struct__(
      __logic_struct__ &&__other
    ) : __logic_struct__(ch::internal::logic_accessor::move(__other)) {}

    __logic_struct__ &operator=(const __logic_struct__ &__other)
    {
      ch::internal::logic_accessor::assign(*this, __other);
      return *this;
    }

    __logic_struct__ &operator=(__logic_struct__ &&__other)
    {
      ch::internal::logic_accessor::move(*this, std::move(__other));
      return *this;
    }

  protected:
    const ch::internal::logic_buffer &__buffer() const { 
      return ch::internal::logic_accessor::source(data);
    }

    friend ch_ostream &operator<<(ch_ostream &__out, const __logic_struct__ &__in)
    {
      __out << "(";
      if (0)
      {
        __out << ", ";
      }
      __out << "ready"
            << "=" << __in.ready;
      if (1)
      {
        __out << ", ";
      }
      __out << "valid"
            << "=" << __in.valid;
      if (2)
      {
        __out << ", ";
      }
      __out << "data"
            << "=" << __in.data;
      __out << ")";
      return __out;
    }
    friend class ch::internal::logic_accessor;

  public:
    template <typename __R>
    auto as() const
    {
      _Static_assert(ch::internal::is_logic_type_v<__R>, "invalid type");
      auto self = reinterpret_cast<const __logic_struct__ *>(this);
      return ch::internal::logic_accessor::cast<std::add_const_t<__R>>(*self);
    }
    template <typename __R>
    auto as()
    {
      _Static_assert(ch::internal::is_logic_type_v<__R>, "invalid type");
      auto self = reinterpret_cast<const __logic_struct__ *>(this);
      return ch::internal::logic_accessor::cast<__R>(*self);
    }
    auto as_bit() const {
      return this->template as<ch_bit<__logic_struct__::traits::bitwidth>>();
    }
    auto as_bit() {
      return this->template as<ch_bit<__logic_struct__::traits::bitwidth>>();
    }
    auto as_int() const {
      return this->template as<ch_int<__logic_struct__::traits::bitwidth>>();
    }
    auto as_int() {
      return this->template as<ch_int<__logic_struct__::traits::bitwidth>>();
    }
    auto as_uint() const {
      return this->template as<ch_uint<__logic_struct__::traits::bitwidth>>();
    }
    auto as_uint() {
      return this->template as<ch_uint<__logic_struct__::traits::bitwidth>>();
    }
    auto as_reg(
      const ch::internal::source_info &srcinfo = ch::internal::source_info(
        ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
      )
    )
    {
      auto self = reinterpret_cast<__logic_struct__ *>(this);
      ch_reg<__logic_struct__> s(srcinfo);
      *self = s;
      return s;
    }
    auto as_reg(
      const __logic_struct__ &init,
      const ch::internal::source_info &srcinfo = ch::internal::source_info(
        ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
      )
    )
    {
      auto self = reinterpret_cast<__logic_struct__ *>(this);
      ch_reg<__logic_struct__> s(init, srcinfo);
      *self = s;
      return s;
    }
    auto ref(
      const ch::internal::source_info &srcinfo = ch::internal::source_info(
        ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
      )
    )
    {
      auto self = reinterpret_cast<__logic_struct__ *>(this);
      return ch::internal::logic_accessor::sliceref<__logic_struct__>(*self, 0, srcinfo);
    }
    auto clone(
      const ch::internal::source_info &srcinfo = ch::internal::source_info(
        ch::internal::source_location("include/htl/elastic.h", 15, 0), ""
      )
    ) const
    {
      auto self = reinterpret_cast<const __logic_struct__ *>(this);
      return ch::internal::logic_accessor::clone(*self, srcinfo);
    }
  };

public:
  using traits = ch::internal::logic_io_traits<(
      ch_width_v<ch::internal::identity_t<ch_in<T>>>
      + ch_width_v<ch::internal::identity_t<ch_in<ch_bool>>>
      + ch_width_v<ch::internal::identity_t<ch_out<ch_bool>>>
    ),
    ch_direction(
      ch::internal::identity_t<ch_in<T>>::traits::direction
      | ch::internal::identity_t<ch_in<ch_bool>>::traits::direction
      | ch::internal::identity_t<ch_out<ch_bool>>::traits::direction
    ),
    ch_elastic, __logic_flip_io__, __system_io__, __system_flip_io__,
    __system_struct__, __logic_struct__
  >;

  ch::internal::identity_t<ch_in<T>> data;
  ch::internal::identity_t<ch_in<ch_bool>> valid;
  ch::internal::identity_t<ch_out<ch_bool>> ready;

  explicit ch_elastic(
    const std::string &name = "io",
    const ch::internal::source_location &sloc = ch::internal::source_location(
      "include/htl/elastic.h", 15, 0
    )
  ) : data(ch::internal::stringf("%s.%s", name.c_str(), "data"), sloc),
      valid(ch::internal::stringf("%s.%s", name.c_str(), "valid"), sloc),
      ready(ch::internal::stringf("%s.%s", name.c_str(), "ready"), sloc) {}

  explicit ch_elastic(
    const typename traits::flip_io &__other, 
    const ch::internal::source_location &sloc = ch::internal::source_location(
      "include/htl/elastic.h", 15, 0
    )
  ) : data(__other.data, sloc), valid(__other.valid, sloc),
      ready(__other.ready, sloc) {}

  explicit ch_elastic(
    const typename traits::system_flip_io &__other,
    const ch::internal::source_location &sloc = ch::internal::source_location(
      "include/htl/elastic.h", 15, 0
    )
  ) : data(__other.data, sloc), valid(__other.valid, sloc),
      ready(__other.ready, sloc) {}

  ch_elastic(
    const ch_elastic &__other
  ) : data(__other.data), valid(__other.valid), ready(__other.ready) {}

  ch_elastic(
    ch_elastic &&__other
  ) : data(std::move(__other.data)), valid(std::move(__other.valid)),
      ready(std::move(__other.ready)) {}

  void operator()(typename traits::flip_io &__other)
  {
    this->data(__other.data);
    this->valid(__other.valid);
    this->ready(__other.ready);
  }

protected:
  ch_elastic &operator=(const ch_elastic &) = delete;
  ch_elastic &operator=(ch_elastic &&) = delete;
  friend ch_ostream &operator<<(ch_ostream &__out, const ch_elastic &__in)
  {
    __out << "(";
    if (0)
    {
      __out << ", ";
    }
    __out << "ready"
          << "=" << __in.ready;
    if (1)
    {
      __out << ", ";
    }
    __out << "valid"
          << "=" << __in.valid;
    if (2)
    {
      __out << ", ";
    }
    __out << "data"
          << "=" << __in.data;
    __out << ")";
    return __out;
  }
};

} // namespace htl
} // namespace ch

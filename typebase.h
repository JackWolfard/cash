#pragma once

#include "common.h"

namespace chdl_internal {

template <typename T, unsigned N> class const_slice_ref;
template <typename T, unsigned N> class slice_ref;

template <typename T> class const_subscript_ref;
template <typename T> class subscript_ref;

template <typename T>
struct partition {
  const T& data;
  uint32_t offset;
  uint32_t length;
};

template <unsigned N, typename T>
class typebase {
public:   
  static const unsigned bit_count = N;
  typedef T data_type;
  
  typebase() {}
  virtual ~typebase() {}
  
  template <unsigned M> const slice_ref<typebase, M>& slice(size_t index) const {
    return const_slice_ref<typebase, M>(*this, index);
  }
  
  template <unsigned M> slice_ref<typebase, M>& slice(size_t index) {
    return slice_ref<typebase, M>(*this, index);
  }
  
  template <unsigned M> const slice_ref<typebase, M>& aslice(size_t index) const {
    return const_slice_ref<typebase, M>(*this, index * M);
  }
  
  template <unsigned M> slice_ref<typebase, M>& aslice(size_t index) {
    return slice_ref<typebase, M>(*this, index * M);
  }
  
  const subscript_ref<typebase>& operator[](size_t index) const {
    return const_subscript_ref<typebase>(*this, index); 
  }
  
  subscript_ref<typebase>& operator[](size_t index) {
    return subscript_ref<typebase>(*this, index); 
  }
  
  typebase& operator=(const typebase& rhs) {
    std::vector< partition<data_type> > data;
    rhs.read(data, 0, N);
    this->write(0, data, 0, N);
    return *this;
  }
  
protected:

  virtual void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const = 0;
  virtual void write(size_t dst_offset, const std::vector< partition<data_type> >& data, size_t src_offset, size_t src_length) = 0;
  
  template <typename T_> friend void read_data(const T_& t, std::vector< partition<typename T_::data_type> >& out, size_t offset, size_t length);    
  template <typename T_> friend void write_data(T_& t, size_t dst_offset, const std::vector< partition<typename T_::data_type> >& data, size_t src_offset, size_t src_length);    
};

template <typename T>
void read_data(const T& t, std::vector< partition<typename T::data_type> >& out, size_t offset, size_t length) {
  reinterpret_cast<const typebase<T::bit_count, typename T::data_type>&>(t).read(out, offset, length);
}

template <typename T>
void write_data(T& t, size_t dst_offset, const std::vector< partition<typename T::data_type> >& data, size_t src_offset, size_t src_length) {
  reinterpret_cast<typebase<T::bit_count,typename T::data_type>&>(t).write(dst_offset, data, src_offset, src_length);
}

///////////////////////////////////////////////////////////////////////////////

template <unsigned N, typename T>
class const_refbase: public typebase<N, T> {
public:   
  using base = typebase<N, T>;
  static const unsigned bit_count = N;
  typedef T data_type;
  
  template <unsigned M> slice_ref<base, M> slice(size_t index) = delete;
  template <unsigned M> slice_ref<base, M> aslice(size_t index) = delete;
  subscript_ref<base> operator[](size_t index) = delete;  
  
private:
  void write(size_t dst_offset, const std::vector< partition<data_type> >& data, size_t src_offset, size_t src_length) override {
    assert(false); // invalid call
  }
};

template <unsigned N, typename T>
class  refbase: public typebase<N, T> {
public:   
  using base = typebase<N, T>;
  using base::operator=;
  static const unsigned bit_count = N;
  typedef T data_type;  
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class const_subscript_ref : public const_refbase<1, typename T::data_type> {
public:
  using base = const_refbase<1, typename T::data_type>;
  typedef typename base::data_type data_type;
  typedef T container_type;
    
  template <unsigned M> const_slice_ref<base, M> slice(size_t index) const = delete;   
  template <unsigned M> const_slice_ref<base, M> aslice(size_t index) const = delete;    
  const_subscript_ref<base> operator[](size_t index) const = delete;  

protected: 
  
  const_subscript_ref(const T& container, size_t index)
    : m_container(container)
    , m_index(index) {
    CHDL_CHECK(index < T::bit_count, "invalid subscript index");
  } 
  
  void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const override {
    assert(offset == 0 && length == 1);
    read_data(m_container, out, m_index, 1);
  }  

  const T& m_container;
  size_t m_index;
  
  friend T;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class subscript_ref : public refbase<1, typename T::data_type> {
public:
  using base = refbase<1, typename T::data_type>;
  using base::base::operator=;
  typedef typename base::data_type data_type;
  typedef T container_type;
    
  template <unsigned M> slice_ref<base, M> slice(size_t index) = delete;   
  template <unsigned M> slice_ref<base, M> aslice(size_t index) = delete;   
  subscript_ref<base> operator[](size_t index) = delete;  
  
  subscript_ref& operator=(const subscript_ref& rhs) {
    m_container = rhs.m_container;
    m_index = rhs.m_index;
    return *this;
  }

protected: 
  
  subscript_ref(T& container, size_t index)
    : m_container(container)
    , m_index(index) {
    CHDL_CHECK(index < T::bit_count, "invalid subscript index");
  } 
  
  void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const override {
    assert(offset == 0 && length == 1);
    read_data(m_container, out, m_index, 1);
  }  
  
  void write(size_t dst_offset, const std::vector< partition<data_type> >& data, size_t src_offset, size_t src_length) override {
    assert(dst_offset == 0 && src_length == 1);
    write_data(m_container, m_index, data, src_offset, 1);
  }

  T& m_container;
  size_t m_index;
  
  friend T;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class const_slice_ref : public const_refbase<N, typename T::data_type> {
public:
  using base = const_refbase<N, typename T::data_type>;
  typedef typename base::data_type data_type;
  typedef T container_type;

protected:

  const_slice_ref(const T& container, size_t start)
    : m_container(container)
    , m_start(start) {
    CHDL_CHECK(start + N <= T::bit_count, "invalid slice range");
  }
  
  void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const override {
    CHDL_CHECK(offset + length <= N, "invalid slice read range");
    read_data(m_container, out, m_start + offset, length);
  }

  const T& m_container;
  size_t m_start;
  
  friend T;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, unsigned N>
class slice_ref : public refbase<N, typename T::data_type> {
public:
  using base = refbase<N, typename T::data_type>;
  using base::base::operator=;
  typedef typename base::data_type data_type;
  typedef T container_type;
  
  slice_ref& operator=(const slice_ref& rhs) {
    m_container = rhs.m_container;
    m_start = rhs.m_start;
    return *this;
  }

protected:

  slice_ref(T& container, size_t start)
    : m_container(container)
    , m_start(start) {
    CHDL_CHECK(start + N <= T::bit_count, "invalid slice range");
  }
  
  void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const override {
    CHDL_CHECK(offset + length <= N, "invalid slice read range");
    read_data(m_container, out, m_start + offset, length);
  }

  void write(size_t dst_offset, const std::vector< partition<data_type> >& data, size_t src_offset, size_t src_length) override {
    CHDL_CHECK(dst_offset + src_length <= N, "invalid slice write range");
    write_data(m_container, m_start + dst_offset, data, src_offset, src_length);
  }

  T& m_container;
  size_t m_start;

  friend T;
};

///////////////////////////////////////////////////////////////////////////////

template <typename B, typename A>
class const_concat_ref : public const_refbase<B::bit_count + A::bit_count, typename A::data_type> {
public:
  static_assert(std::is_same<typename A::data_type, typename B::data_type>::value, "type mismatch!");
  using base = const_refbase<B::bit_count + A::bit_count, typename A::data_type>;
  typedef typename A::data_type data_type;
  typedef A first_container_type;
  typedef B second_container_type;

protected:
   
  const_concat_ref(const B& b, const A& a)
    : m_b(b)
    , m_a(a)
  {}

  void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const override {
    CHDL_CHECK(offset + length <= const_concat_ref::bit_count, "invalid concat read range");
    if (offset + length <= A::bit_count)
      read_data(m_a, out, offset, length);
    else if (offset >= A::bit_count)
      read_data(m_b, out, offset - A::bit_count, length);
    else {
      size_t len = A::bit_count - offset;
      read_data(m_a, out, offset, len);
      read_data(m_b, out, 0, length - len);
    }
  }

  const A& m_a;
  const B& m_b;
  
  template <typename B_, typename A_>
  friend const_concat_ref<B_, A_> make_const_concat_ref(const B_& b, const A_& a);
};

template <typename B, typename A>
const_concat_ref<B, A> make_const_concat_ref(const B& b, const A& a) {
  return const_concat_ref<B, A>(b, a);
}

///////////////////////////////////////////////////////////////////////////////

template <typename B, typename A>
class concat_ref : public refbase<B::bit_count + A::bit_count, typename A::data_type> {
public:
  static_assert(std::is_same<typename A::data_type, typename B::data_type>::value, "type mismatch!");
  using base = refbase<B::bit_count + A::bit_count, typename A::data_type>;
  using base::base::operator=;
  typedef typename A::data_type data_type;
  typedef A first_container_type;
  typedef B second_container_type; 
  
  concat_ref& operator=(const concat_ref& rhs) {
    m_a = rhs.m_a;
    m_b = rhs.m_b;
    return *this;
  }

protected:
  
  concat_ref(const B& b, const A& a)
    : m_b(const_cast<B&>(b))
    , m_a(const_cast<A&>(a))
  {}
  
  void read(std::vector< partition<data_type> >& out, size_t offset, size_t length) const override {
    CHDL_CHECK(offset + length <= concat_ref::bit_count, "invalid concat read range");
    if (offset + length <= A::bit_count)
      read_data(m_a, out, offset, length);
    else if (offset >= A::bit_count)
      read_data(m_b, out, offset - A::bit_count, length);
    else {
      size_t len = A::bit_count - offset;
      read_data(m_a, out, offset, len);
      read_data(m_b, out, 0, length - len);
    }
  }
  
  void write(size_t dst_offset, const std::vector< partition<data_type> >& data, size_t src_offset, size_t src_length) override {
    CHDL_CHECK(dst_offset + src_length <= concat_ref::bit_count, "invalid concat write range");
    if (dst_offset + src_length <= A::bit_count)
      write_data(m_a, dst_offset, data, src_offset, src_length);
    else if (dst_offset >= A::bit_count)
      write_data(m_b, dst_offset - A::bit_count, data, src_offset, src_length);
    else {
      size_t len = A::bit_count - dst_offset;
      write_data(m_a, dst_offset, data, src_offset, len);
      write_data(m_b, 0, data, src_offset + len, src_length - len);
    }
  }

  A& m_a;
  B& m_b;
  
  template <typename B_, typename A_>
  friend concat_ref<B_, A_> make_concat_ref(const B_& b, const A_& a);
};

template <typename B, typename A>
concat_ref<B, A> make_concat_ref(const B& b, const A& a) {
  return concat_ref<B, A>(b, a);
}

}

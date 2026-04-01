#ifndef SRC_HPP
#define SRC_HPP

#include <stdexcept>
#include <initializer_list>

namespace sjtu {

class any_ptr {
 private:
  struct holder_base {
    virtual ~holder_base() = default;
  };

  template <class T>
  struct holder : holder_base {
    T *ptr;
    explicit holder(T *p) : ptr(p) {}
    ~holder() override { delete ptr; }
  };

  struct control_block {
    holder_base *storage;
    std::size_t refcnt;
    explicit control_block(holder_base *s) : storage(s), refcnt(1) {}
  };

  control_block *ctrl = nullptr;

  void retain() {
    if (ctrl) ++ctrl->refcnt;
  }

  void release() {
    if (!ctrl) return;
    if (--ctrl->refcnt == 0) {
      delete ctrl->storage;
      delete ctrl;
    }
    ctrl = nullptr;
  }

 public:
  // 默认构造：空指针语义
  any_ptr() = default;

  // 拷贝构造：浅拷贝，共享同一份内存
  any_ptr(const any_ptr &other) : ctrl(other.ctrl) { retain(); }

  // 由裸指针构造：接管生命周期（允许隐式转换）
  template <class T>
  any_ptr(T *ptr) : ctrl(nullptr) {
    if (ptr) ctrl = new control_block(new holder<T>(ptr));
  }

  // 析构：最后一个对象释放内存
  ~any_ptr() { release(); }

  // 拷贝赋值：浅拷贝
  any_ptr &operator=(const any_ptr &other) {
    if (this == &other) return *this;
    // 先保留对方，避免自赋值与别名问题
    control_block *new_ctrl = other.ctrl;
    if (new_ctrl) ++new_ctrl->refcnt;
    release();
    ctrl = new_ctrl;
    return *this;
  }

  // 从裸指针赋值：接管生命周期
  template <class T>
  any_ptr &operator=(T *ptr) {
    release();
    if (ptr)
      ctrl = new control_block(new holder<T>(ptr));
    else
      ctrl = nullptr;
    return *this;
  }

  // 取出引用；类型不匹配抛出 std::bad_cast
  template <class T>
  T &unwrap() {
    if (!ctrl || !ctrl->storage) throw std::bad_cast();
    auto *h = dynamic_cast<holder<T> *>(ctrl->storage);
    if (!h || !h->ptr) throw std::bad_cast();
    return *(h->ptr);
  }

  template <class T>
  const T &unwrap() const {
    if (!ctrl || !ctrl->storage) throw std::bad_cast();
    auto *h = dynamic_cast<const holder<T> *>(ctrl->storage);
    if (!h || !h->ptr) throw std::bad_cast();
    return *(h->ptr);
  }
};

// 由值构造
template <class T>
any_ptr make_any_ptr(const T &t) {
  return any_ptr(new T(t));
}

// 可变参数构造转发
template <class T, class... Args>
any_ptr make_any_ptr(Args &&...args) {
  return any_ptr(new T(std::forward<Args>(args)...));
}

// initializer_list 构造（如 std::vector / std::map 等）
template <class T>
any_ptr make_any_ptr(std::initializer_list<typename T::value_type> il) {
  return any_ptr(new T(il));
}

}  // namespace sjtu

#endif

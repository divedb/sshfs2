#pragma once

#include <memory>
#include <cassert>

namespace sshfs2 {

template <typename T, typename D = std::default_delete<T>>
class SharedPtr {
 public:
  SharedPtr() = default;

  explicit SharedPtr(T* ptr, D d = D()) : ptr_(ptr), deleter_(d) {
    count_ = new int(1);
  }

  SharedPtr(const SharedPtr& other)
      : ptr_(other.ptr_), count_(other.count_), deleter_(other.deleter_) {
    ++*count_;
  }

  SharedPtr(SharedPtr&& other) noexcept { MoveFrom(std::move(other)); }

  ~SharedPtr() { Release(); }

  SharedPtr& operator=(const SharedPtr& other) {
    if (this == &other) {
      return *this;
    }

    Release();
    ptr_ = other.ptr_;
    count_ = other.count_;
    deleter_ = other.deleter_;
    ++*count_;

    return *this;
  }

  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this == &other) {
      return *this;
    }

    Release();
    MoveFrom(std::move(other));

    return *this;
  }

  SharedPtr& operator=(T* ptr) {
    Release();

    count_ = new int(1);
    ptr_ = ptr;

    return *this;
  }

  operator bool() const { return ptr_ != nullptr; }

  int Count() const {
    assert(count_ && "Invalid count_");

    return *count_;
  }

  T* operator->() const { return ptr_; }

 private:
  void Release() {
    if (count_ == nullptr) {
      return;
    }

    if (--(*count_) == 0) {
      deleter_(ptr_);
      delete count_;
    }
  }

  void MoveFrom(SharedPtr&& other) noexcept {
    ptr_ = std::exchange(other.ptr_, nullptr);
    count_ = std::exchange(other.count_, nullptr);
    deleter_ = std::move(other.deleter_);
  }

  T* ptr_ = nullptr;
  int* count_ = nullptr;
  D deleter_;
};

}  // namespace sshfs2
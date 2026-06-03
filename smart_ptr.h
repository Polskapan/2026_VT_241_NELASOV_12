#pragma once

#include <utility>

template <typename T>
class SmartPtr {
public:
    explicit SmartPtr(T* ptr = nullptr) noexcept : ptr_(ptr) {}

    ~SmartPtr() {
        delete ptr_;
    }

    SmartPtr(const SmartPtr&) = delete;
    SmartPtr& operator=(const SmartPtr&) = delete;

    SmartPtr(SmartPtr&& other) noexcept : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    SmartPtr& operator=(SmartPtr&& other) noexcept {
        if (this != &other) {
            reset();
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    T* get() const noexcept { return ptr_; }
    T& operator*() const { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }

    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    T* release() noexcept {
        T* tmp = ptr_;
        ptr_ = nullptr;
        return tmp;
    }

    void reset(T* ptr = nullptr) noexcept {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }

private:
    T* ptr_;
};

template <typename T, typename... Args>
SmartPtr<T> make_smart(Args&&... args) {
    return SmartPtr<T>(new T(std::forward<Args>(args)...));
}
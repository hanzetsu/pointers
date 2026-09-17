#pragma once
#include <cstddef>
#include <type_traits>
#include "SharedPtr.hpp"

template <typename T>
class WeakPtr
{
private:
    T *ptr;
    ControlBlock<T> *cb;

    template <typename U> friend class WeakPtr;
    template <typename U> friend class SharedPtr;

    void release() noexcept
    {
        if (cb == nullptr)
            return;
        if (--cb->weak_count == 0 && cb->shared_count == 0)
            delete cb;
        ptr = nullptr;
        cb = nullptr;
    }

public:
    WeakPtr() noexcept : ptr(nullptr), cb(nullptr) {}

    WeakPtr(const SharedPtr<T> &sp) noexcept
        : ptr(sp.ptr), cb(sp.cb)
    {
        if (cb != nullptr)
            ++cb->weak_count;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    WeakPtr(const SharedPtr<U> &sp) noexcept
        : ptr(sp.ptr), cb(sp.cb)
    {
        if (cb != nullptr)
            ++cb->weak_count;
    }

    WeakPtr(const WeakPtr &other) noexcept
        : ptr(other.ptr), cb(other.cb)
    {
        if (cb != nullptr)
            ++cb->weak_count;
    }

    WeakPtr(WeakPtr &&other) noexcept
        : ptr(other.ptr), cb(other.cb)
    {
        other.ptr = nullptr;
        other.cb = nullptr;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    WeakPtr(const WeakPtr<U> &other) noexcept
        : ptr(other.ptr), cb(other.cb)
    {
        if (cb != nullptr)
            ++cb->weak_count;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    WeakPtr(WeakPtr<U> &&other) noexcept
        : ptr(other.ptr), cb(other.cb)
    {
        other.ptr = nullptr;
        other.cb = nullptr;
    }

    WeakPtr &operator=(const WeakPtr &other) noexcept
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            cb = other.cb;
            if (cb != nullptr)
                ++cb->weak_count;
        }
        return *this;
    }

    WeakPtr &operator=(WeakPtr &&other) noexcept
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            cb = other.cb;
            other.ptr = nullptr;
            other.cb = nullptr;
        }
        return *this;
    }

    WeakPtr &operator=(const SharedPtr<T> &sp) noexcept
    {
        if (cb != sp.cb)
        {
            release();
            ptr = sp.ptr;
            cb = sp.cb;
            if (cb != nullptr)
                ++cb->weak_count;
        }
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    WeakPtr &operator=(const SharedPtr<U> &sp) noexcept
    {
        if (cb != sp.cb)
        {
            release();
            ptr = sp.ptr;
            cb = sp.cb;
            if (cb != nullptr)
                ++cb->weak_count;
        }
        return *this;
    }

    ~WeakPtr()
    {
        release();
    }

    bool expired() const noexcept
    {
        return cb == nullptr || cb->shared_count == 0;
    }

    SharedPtr<T> lock() const
    {
        if (expired())
            return SharedPtr<T>();
        SharedPtr<T> sp;
        sp.ptr = cb->ptr;
        sp.cb = cb;
        ++cb->shared_count;
        return sp;
    }

    void reset() noexcept
    {
        release();
    }

    size_t getCountRef() const noexcept
    {
        return cb != nullptr ? cb->shared_count : 0;
    }

    size_t getCountWeak() const noexcept
    {
        return cb != nullptr ? cb->weak_count : 0;
    }
};
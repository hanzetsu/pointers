#pragma once
#include <cstddef>
#include <type_traits>
#include "Exceptions.hpp"

template <typename T> class SharedPtr;
template <typename T> class WeakPtr;

template <typename T>
struct ControlBlock
{
    T *ptr;
    size_t shared_count;
    size_t weak_count;

    ControlBlock(T *p) : ptr(p), shared_count(1), weak_count(0) {}
};

template <typename T>
class SharedPtr
{
private:
    T *ptr;
    ControlBlock<T> *cb;

    template <typename U> friend class SharedPtr;
    template <typename U> friend class WeakPtr;

    void release() noexcept
    {
        if (cb == nullptr)
            return;
        if (--cb->shared_count == 0)
        {
            delete cb->ptr;
            cb->ptr = nullptr;
            if (cb->weak_count == 0)
                delete cb;
        }
        ptr = nullptr;
        cb = nullptr;
    }

public:
    SharedPtr(T *p = nullptr) : ptr(p), cb(nullptr)
    {
        if (p != nullptr)
            cb = new ControlBlock<T>(p);
    }

    SharedPtr(const SharedPtr &other)
        : ptr(other.ptr), cb(other.cb)
    {
        if (cb != nullptr)
            ++cb->shared_count;
    }

    SharedPtr(SharedPtr &&other) noexcept
        : ptr(other.ptr), cb(other.cb)
    {
        other.ptr = nullptr;
        other.cb = nullptr;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(const SharedPtr<U> &other)
        : ptr(other.ptr), cb(other.cb)
    {
        if (cb != nullptr)
            ++cb->shared_count;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(SharedPtr<U> &&other) noexcept
        : ptr(other.ptr), cb(other.cb)
    {
        other.ptr = nullptr;
        other.cb = nullptr;
    }

    SharedPtr &operator=(const SharedPtr &other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            cb = other.cb;
            if (cb != nullptr)
                ++cb->shared_count;
        }
        return *this;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept
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

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr &operator=(const SharedPtr<U> &other)
    {
        if (ptr != other.ptr)
        {
            release();
            ptr = other.ptr;
            cb = other.cb;
            if (cb != nullptr)
                ++cb->shared_count;
        }
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr &operator=(SharedPtr<U> &&other) noexcept
    {
        if (ptr != other.ptr)
        {
            release();
            ptr = other.ptr;
            cb = other.cb;
            other.ptr = nullptr;
            other.cb = nullptr;
        }
        return *this;
    }

    ~SharedPtr()
    {
        release();
    }

    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr; }
    T *get() const noexcept { return ptr; }

    size_t getCountRef() const noexcept
    {
        return cb != nullptr ? cb->shared_count : 0;
    }

    void reset(T *p = nullptr)
    {
        if (ptr != p)
        {
            release();
            ptr = p;
            if (p != nullptr)
                cb = new ControlBlock<T>(p);
        }
    }
};

/////////////////////////////////////////

template <typename T>
class SharedPtrArr
{
private:
    T *ptr;
    size_t *ref_count;
    size_t size;

    void release() noexcept
    {
        if (ref_count != nullptr && --(*ref_count) == 0)
        {
            delete[] ptr;
            delete ref_count;
        }
        ptr = nullptr;
        ref_count = nullptr;
        size = 0;
    }

public:
    SharedPtrArr(T *p = nullptr, size_t s = 0) : ptr(p), ref_count(nullptr), size(s)
    {
        if (p != nullptr)
            ref_count = new size_t(1);
    }

    SharedPtrArr(const SharedPtrArr &other)
        : ptr(other.ptr), ref_count(other.ref_count), size(other.size)
    {
        if (ref_count != nullptr)
            ++(*ref_count);
    }

    SharedPtrArr(SharedPtrArr &&other) noexcept
        : ptr(other.ptr), ref_count(other.ref_count), size(other.size)
    {
        other.ptr = nullptr;
        other.ref_count = nullptr;
        other.size = 0;
    }

    SharedPtrArr &operator=(const SharedPtrArr &other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            size = other.size;
            if (ref_count != nullptr)
                ++(*ref_count);
        }
        return *this;
    }

    SharedPtrArr &operator=(SharedPtrArr &&other) noexcept
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            size = other.size;
            other.ptr = nullptr;
            other.ref_count = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~SharedPtrArr()
    {
        release();
    }

    T &operator[](size_t index)
    {
        if (ptr == nullptr || index >= size)
            throw IndexOutOfRange(index, size, "SharedPtrArr::operator[]: индекс вне диапазона");
        return ptr[index];
    }

    const T &operator[](size_t index) const
    {
        if (ptr == nullptr || index >= size)
            throw IndexOutOfRange(index, size, "SharedPtrArr::operator[]: индекс вне диапазона");
        return ptr[index];
    }

    T *get() const noexcept { return ptr; }
    size_t getSize() const noexcept { return size; }

    size_t getCountRef() const noexcept
    {
        return ref_count != nullptr ? *ref_count : 0;
    }

    void reset(T *p = nullptr, size_t s = 0)
    {
        if (ptr != p || size != s)
        {
            release();
            ptr = p;
            size = s;
            if (p != nullptr)
                ref_count = new size_t(1);
        }
    }
};
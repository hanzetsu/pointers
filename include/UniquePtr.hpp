#pragma once
#include <cstddef>
#include <type_traits>
#include "Exceptions.hpp"

template <typename T>
class UniquePtr
{
private:
    T *ptr;

    template <typename U> friend class UniquePtr;

    void destroy() noexcept
    {
        delete ptr;
        ptr = nullptr;
    }

public:
    UniquePtr(T *p = nullptr) noexcept : ptr(p) {}

    ~UniquePtr() { destroy(); }

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    UniquePtr(UniquePtr &&other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            destroy();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    UniquePtr(UniquePtr<U> &&other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
    UniquePtr &operator=(UniquePtr<U> &&other) noexcept
    {
        if (ptr != other.ptr)
        {
            destroy();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T &operator*() const { return *ptr; }
    T *operator->() const noexcept { return ptr; }
    T *get() const noexcept { return ptr; }

    T *release() noexcept
    {
        T *tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T *p = nullptr) noexcept
    {
        if (ptr != p)
        {
            destroy();
            ptr = p;
        }
    }
};

/////////////////////////////////////////

template <typename T>
class UniquePtrArr
{
private:
    T *ptr;

    void destroy() noexcept
    {
        delete[] ptr;
        ptr = nullptr;
    }

public:
    UniquePtrArr(T *p = nullptr) noexcept : ptr(p) {}

    ~UniquePtrArr() { destroy(); }

    UniquePtrArr(const UniquePtrArr &) = delete;
    UniquePtrArr &operator=(const UniquePtrArr &) = delete;

    UniquePtrArr(UniquePtrArr &&other) noexcept : ptr(other.ptr)
    {
        other.ptr = nullptr;
    }

    UniquePtrArr &operator=(UniquePtrArr &&other) noexcept
    {
        if (this != &other)
        {
            destroy();
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    T &operator[](size_t index) { return ptr[index]; }
    const T &operator[](size_t index) const { return ptr[index]; }

    T *get() const noexcept { return ptr; }

    T *release() noexcept
    {
        T *tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    void reset(T *p = nullptr) noexcept
    {
        if (ptr != p)
        {
            destroy();
            ptr = p;
        }
    }
};
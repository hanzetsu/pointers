#include <cstddef>
#include "Exceptions.hpp"  

template <typename T>
class UniquePtr
{
private:
    T *ptr;

    void release() noexcept
    {
        delete ptr;
        ptr = nullptr;
    }

public:
    UniquePtr(T *p = nullptr) noexcept : ptr(p) {}

    ~UniquePtr() { release(); }

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
            release();
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
            release();
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

    void release() noexcept
    {
        delete[] ptr;
        ptr = nullptr;
    }

public:
    UniquePtrArr(T *p = nullptr) noexcept : ptr(p) {}

    ~UniquePtrArr() { release(); }

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
            release();
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
            release();
            ptr = p;
        }
    }
};
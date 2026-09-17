#include <cstddef>
#include <type_traits>
#include "Exceptions.hpp"

template <typename T>
class SharedPtr
{
private:
    T *ptr;
    size_t *ref_count;

    template <typename U>
    friend class SharedPtr;

    void release() noexcept
    {
        if (ref_count != nullptr && --(*ref_count) == 0)
        {
            delete ptr;
            delete ref_count;
        }
        ptr = nullptr;
        ref_count = nullptr;
    }

public:
    SharedPtr(T *p = nullptr) : ptr(p), ref_count(nullptr)
    {
        if (p != nullptr)
            ref_count = new size_t(1);
    }

    SharedPtr(const SharedPtr &other)
        : ptr(other.ptr), ref_count(other.ref_count)
    {
        if (ref_count != nullptr)
            ++(*ref_count);
    }

    SharedPtr(SharedPtr &&other) noexcept
        : ptr(other.ptr), ref_count(other.ref_count)
    {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(const SharedPtr<U> &other)
        : ptr(other.ptr), ref_count(other.ref_count)
    {
        if (ref_count != nullptr)
            ++(*ref_count);
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible_v<U *, T *>>>
    SharedPtr(SharedPtr<U> &&other) noexcept
        : ptr(other.ptr), ref_count(other.ref_count)
    {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    SharedPtr &operator=(const SharedPtr &other)
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count != nullptr)
                ++(*ref_count);
        }
        return *this;
    }

    SharedPtr &operator=(SharedPtr &&other) noexcept
    {
        if (this != &other)
        {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
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
            ref_count = other.ref_count;
            if (ref_count != nullptr)
                ++(*ref_count);
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
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
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
        return ref_count != nullptr ? *ref_count : 0;
    }

    void reset(T *p = nullptr)
    {
        if (ptr != p)
        {
            release();
            ptr = p;
            if (p != nullptr)
                ref_count = new size_t(1);
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
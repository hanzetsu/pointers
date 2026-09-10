template <typename T>
class SharedPtr
{
private:
    T *ptr;
    int *ref_count;

public:
    SharedPtr(T *p = nullptr) : ptr(p), ref_count(new int(1)) {}

    SharedPtr(const SharedPtr &other)
        : ptr(other.ptr), ref_count(other.ref_count)
    {
        ++(*ref_count);
    }

    SharedPtr &operator=(const SharedPtr &other)
    {
        if (this != &other)
        {
            if (--(*ref_count) == 0)
            {
                delete ptr;
                delete ref_count;
            }
            ptr = other.ptr;
            ref_count = other.ref_count;
            ++(*ref_count);
        }
        return *this;
    }

    ~SharedPtr()
    {
        if (--(*ref_count) == 0)
        {
            delete ptr;
            delete ref_count;
        }
    }

    T &operator*() const { return *ptr; }
    T *operator->() const { return ptr; }
    T *get() const { return ptr; }

    void reset(T *p = nullptr)
    {
        if (ptr != p)
        {
            if (--(*ref_count) == 0)
            {
                delete ptr;
                delete ref_count;
            }
            ptr = p;
            if (p != nullptr)
            {
                ref_count = new int(1);
            }
            else
                ref_count = nullptr;
        }
    }
};
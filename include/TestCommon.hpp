#pragma once

#include <iostream>
#include <string>
#include <cstddef>

#include "UniquePtr.hpp"
#include "SharedPtr.hpp"
#include "Exceptions.hpp"
#include "WeakPtr.hpp"

struct Tracked
{
    inline static int alive = 0;
    int value;
    Tracked(int v = 0) : value(v) { ++alive; }
    ~Tracked() { --alive; }
};

struct Base
{
    inline static int alive = 0;
    int base_value = 1;
    Base() { ++alive; }
    virtual ~Base() { --alive; }
};

struct Derived : Base
{
    int derived_value = 2;
    Derived() { ++alive; }
    ~Derived() override { --alive; }
};

struct TestFailure
{
    std::string msg;
};

inline void CHECK(bool cond, std::string msg,
                  const char *file = __builtin_FILE(),
                  int line = __builtin_LINE())
{
    if (!cond)
    {
        std::ostringstream oss;
        oss << "Провал: " << msg << " (" << file << ":" << line << ")";
        throw TestFailure{oss.str()};
    }
}
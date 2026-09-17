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

inline bool CHECK(bool cond, std::string msg,
                  const char *file = __builtin_FILE(),
                  int line = __builtin_LINE())
{
    if (!cond)
    {
        std::cerr << "Провал: " << msg << " (" << file << ":" << line << ")\n";
        return false;
    }
    return true;
}
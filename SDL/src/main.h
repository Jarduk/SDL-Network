#pragma once

#include <iostream>

#include "CNibbles.h"

int g_LeakCounter = 0;

#undef operator new
void* operator new(std::size_t sz)
{
    g_LeakCounter++;
return std::malloc(sz);
}

#undef operator new[]
void* operator new[](std::size_t sz, char c)
{
    g_LeakCounter++;
    return ::operator new(sz);
}

#undef operator delete
void operator delete(void* ptr) noexcept
{
    g_LeakCounter--;
    std::free(ptr);
}

#undef operator delete[]
void operator delete[](void* ptr, std::size_t sz)
{
    g_LeakCounter--;
    ::operator delete(ptr);
}


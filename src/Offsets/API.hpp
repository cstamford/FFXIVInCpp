#pragma once

#include <cstddef>
#include <cstdint>

struct Framework
{
    struct VTable
    {
        uintptr_t fn_0;
        uintptr_t fn_1;
        uintptr_t fn_2;
        uintptr_t fn_3;
        uintptr_t fn_update;
    };

    VTable* vtable;
    std::byte _0[5808];
    float dt; // dt in game time, pausing the window pauses this counter
};

static_assert(offsetof(Framework, dt) == 5816);

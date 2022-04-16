#pragma once

#include <cstdint>

struct OffsetData
{
    // Static globals
    uintptr_t local_player;
    uintptr_t job_gauge;
    uintptr_t action_manager;
    uintptr_t target_manager;

    // Functions
    uintptr_t fn_get_adjusted_icon;
};

struct MemoryData;

OffsetData offsets_init(MemoryData* mem);

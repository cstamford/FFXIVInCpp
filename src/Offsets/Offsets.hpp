#pragma once

#include <Offsets/API.hpp>

using GetAdjustedIcon = uint32_t(*)(const uintptr_t, const uint32_t ability);

struct OffsetData
{
    // Static globals
    uintptr_t local_player;
    uintptr_t job_gauge;
    uintptr_t action_manager;
    uintptr_t target_manager;
    Framework** framework;

    // Functions
    GetAdjustedIcon fn_get_adjusted_icon;
};

struct MemoryData;

OffsetData offsets_init(MemoryData* mem);

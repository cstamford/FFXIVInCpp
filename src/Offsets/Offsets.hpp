#pragma once

#include <Offsets/API.hpp>

using ActionManager_GetRecastGroup = uint32_t(*)(ActionManager*, uint32_t type, uint32_t id);
using ActionManager_GetRecastGroupDetail = RecastGroupDetail*(*)(ActionManager*, uint32_t group);
using ActionManager_GetMaxCharges = uint8_t(*)(uint32_t action_id, uint32_t level);
using ActionManager_UseAction =  bool(*)(ActionManager*, uint32_t type, uint32_t action_id, uint32_t target_id, uint32_t, uint32_t, uint32_t, void*);

struct OffsetData
{
    // Static globals
    Character** local_player;
    JobGauge* job_gauge;
    ActionManager* action_manager;
    TargetManager* target_manager;
    Framework** framework;

    // Functions
    ActionManager_GetRecastGroup fn_get_recast_group;
    ActionManager_GetRecastGroupDetail fn_get_recast_group_detail;
    ActionManager_GetMaxCharges fn_get_max_charges;
    ActionManager_UseAction fn_use_action;
};

struct MemoryData;
OffsetData offsets_init(MemoryData* mem);

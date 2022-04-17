#pragma once

#include <cstdint>

struct Action
{
    const char* name;
    uint32_t id;
    uint8_t level;
    uint8_t primary_cd_group;
    uint8_t secondary_cd_group;
    bool targets_self;
    bool targets_friendly;
    bool targets_hostile;
    uint8_t range;
    uint8_t radius;
    uint8_t max_charges;
};

extern Action ACTIONS[30001];

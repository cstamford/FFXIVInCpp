#pragma once

#include <Offsets/Generated/Actions.hpp>
#include <Offsets/Generated/Jobs.hpp>

#include <cstddef>
#include <cstdint>

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b
#define PAD(n) std::byte CONCAT(_pad, __COUNTER__)[n]

struct RecastGroupDetail
{
PAD(4);
    uint32_t id;
    float elapsed;
    float total;
};

struct JobGauge
{

};

struct Status
{
    uint16_t id;
    uint16_t sub_id;
    float duration;
    uint32_t creator;
};

struct StatusList
{
PAD(8);
    Status statuses[30];
};

struct Character
{
    // TODO: (maybe in action manager?)
    // casting
    // current cast time
    // total cast time

PAD(116);
    uint32_t id;
PAD(20);
    uint8_t type; // 2 if enemy
    uint8_t subtype;
PAD(18);
    float x, y, z; // y is up
PAD(4);
    float rot_radians;
PAD(12);
    float hitbox;
PAD(284);
    Job job;
    uint8_t level;
PAD(2670);
    uint32_t target_id;
PAD(3788);
    StatusList status_list;
};

static_assert(offsetof(Character, id) == 116);
static_assert(offsetof(Character, type) == 140);
static_assert(offsetof(Character, subtype) == 141);
static_assert(offsetof(Character, x) == 160);
static_assert(offsetof(Character, y) == 164);
static_assert(offsetof(Character, z) == 168);
static_assert(offsetof(Character, rot_radians) == 176);
static_assert(offsetof(Character, hitbox) == 192);
static_assert(offsetof(Character, job) == 480);
static_assert(offsetof(Character, level) == 481);
static_assert(offsetof(Character, target_id) == 3152);
static_assert(offsetof(Character, status_list) == 6944);

struct ActionManager
{
PAD(8);
    float animation_lock; // not sure: seems to be though!
PAD(82);
    uint32_t combo_id;
    float combo_timer;
};

static_assert(offsetof(ActionManager, animation_lock) == 8);
static_assert(offsetof(ActionManager, combo_id) == 96);
static_assert(offsetof(ActionManager, combo_timer) == 100);

struct TargetManager
{
PAD(112);
    Character* mouseover;
PAD(8);
    Character* target;
};

static_assert(offsetof(TargetManager, mouseover) == 112);
static_assert(offsetof(TargetManager, target) == 128);

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
PAD(5808);
    float dt; // dt in game time, pausing the window pauses this counter
};

static_assert(offsetof(Framework, dt) == 5816);

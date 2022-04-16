#include <Offsets/Memory.hpp>
#include <Offsets/Offsets.hpp>

#include <cstdio>

MemoryData* g_mem;
OffsetData* g_offsets;

extern "C"
{
    _declspec(dllexport) void project_init(MemoryData* mem, OffsetData* offsets)
    {
        g_mem = mem;
        g_offsets = offsets;

        static GetAdjustedIcon original;
        static constexpr auto hook = [](const uintptr_t _, const uint32_t icon_id) -> uint32_t
        {
            printf("Getting icon with id %d\n", icon_id);
            return original(_, icon_id);
        };

        mem_hook("replace icon", offsets->fn_get_adjusted_icon, (GetAdjustedIcon)hook, (void**)&original);
    }

    _declspec(dllexport) void project_update(Framework* framework, const float dt)
    {
    }

    _declspec(dllexport) void project_free()
    {
        mem_hook_free(g_offsets->fn_get_adjusted_icon);
    }
}

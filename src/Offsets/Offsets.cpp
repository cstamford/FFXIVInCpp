#include <Offsets/Offsets.hpp>
#include <Offsets/Memory.hpp>

#include <cstdio>

OffsetData offsets_init(MemoryData* mem)
{
    printf("\nInitializing offsets.\n");

    OffsetData offsets;

    offsets.local_player = (Character**)mem_rel_operand(mem, mem_scan(mem, "48 89 05 ?? ?? ?? ?? 88 05 ?? ?? ?? ?? 88 05", MemorySectionType::Text));
    offsets.job_gauge = (JobGauge*)mem_rel_operand(mem, mem_scan(mem, "E8 ?? ?? ?? ?? FF C6 48 8D 5B 0C", MemorySectionType::Text) + 0xB9);
    offsets.action_manager = (ActionManager*)mem_rel_operand(mem, mem_scan(mem, "48 8D 0D ?? ?? ?? ?? F3 0F 10 13", MemorySectionType::Text));
    offsets.target_manager = (TargetManager*)mem_rel_operand(mem, mem_scan(mem, "48 8B 05 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? FF 50 ?? 48 85 DB", MemorySectionType::Text) + 0x3);
    offsets.framework = (Framework**)mem_rel_operand<uintptr_t>(mem, mem_scan(mem, "48 C7 05 ?? ?? ?? ?? 00 00 00 00 E8 ?? ?? ?? ?? 48 8D ?? ?? ?? 00 00 E8 ?? ?? ?? ?? 48 8D", MemorySectionType::Text));

    printf("local_player: %llx [%llx]\n", (uintptr_t)offsets.local_player, mem_ida_addr(mem, (uintptr_t)offsets.local_player));
    printf("action_manager: %llx [%llx]\n", (uintptr_t)offsets.action_manager, mem_ida_addr(mem, (uintptr_t)offsets.action_manager));
    printf("job_gauge: %llx [%llx]\n", (uintptr_t)offsets.job_gauge, mem_ida_addr(mem, (uintptr_t)offsets.job_gauge));
    printf("target_manager: %llx [%llx]\n", (uintptr_t)offsets.target_manager, mem_ida_addr(mem, (uintptr_t)offsets.target_manager));
    printf("framework: %llx [%llx]\n", (uintptr_t)offsets.framework, mem_ida_addr(mem, (uintptr_t)offsets.framework));

    offsets.fn_get_recast_group = (ActionManager_GetRecastGroup)mem_scan(mem, "E8 ?? ?? ?? ?? 8B D0 48 8B CD 8B F0", MemorySectionType::Text);
    offsets.fn_get_recast_group_detail = (ActionManager_GetRecastGroupDetail)mem_scan(mem, "40 53 48 83 EC ?? 48 63 DA 85 D2", MemorySectionType::Text);
    offsets.fn_get_max_charges = (ActionManager_GetMaxCharges)mem_scan(mem, "E8 ?? ?? ?? ?? 33 DB 8B C8", MemorySectionType::Text);
    offsets.fn_use_action = (ActionManager_UseAction)mem_scan(mem, "E8 ?? ?? ?? ?? EB 64 B1 01", MemorySectionType::Text);

    printf("fn_get_recast_group: %llx [%llx]\n", (uintptr_t)offsets.fn_get_recast_group, mem_ida_addr(mem, (uintptr_t)offsets.fn_get_recast_group));
    printf("fn_get_recast_group_detail: %llx [%llx]\n", (uintptr_t)offsets.fn_get_recast_group_detail, mem_ida_addr(mem, (uintptr_t)offsets.fn_get_recast_group_detail));
    printf("fn_get_max_charges: %llx [%llx]\n", (uintptr_t)offsets.fn_get_max_charges, mem_ida_addr(mem, (uintptr_t)offsets.fn_get_max_charges));
    printf("fn_use_action: %llx [%llx]\n", (uintptr_t)offsets.fn_use_action, mem_ida_addr(mem, (uintptr_t)offsets.fn_use_action));

    return offsets;
}

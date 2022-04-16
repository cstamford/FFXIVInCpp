#include <Offsets/Offsets.hpp>
#include <Offsets/Memory.hpp>

#include <cstdio>

OffsetData offsets_init(MemoryData* mem)
{
    OffsetData offsets;

    offsets.local_player = mem_lookup_data(mem, mem_scan(mem, "48 89 05 ?? ?? ?? ?? 88 05 ?? ?? ?? ?? 88 05", MemorySectionType::Text));
    offsets.action_manager = mem_lookup_data(mem, mem_scan(mem, "8B D7 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B F8", MemorySectionType::Text));
    offsets.job_gauge = mem_lookup_data(mem, mem_scan(mem, "E8 ?? ?? ?? ?? FF C6 48 8D 5B 0C", MemorySectionType::Text) + 0xB9);
    offsets.target_manager = mem_lookup_data(mem, mem_scan(mem, "48 8B 05 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? FF 50 ?? 48 85 DB", MemorySectionType::Text) + 0x3);

    printf("local_player: %llx [%llx]\n", offsets.local_player, mem_ida_addr(mem, offsets.local_player));
    printf("action_manager: %llx [%llx]\n", offsets.action_manager, mem_ida_addr(mem, offsets.action_manager));
    printf("job_gauge: %llx [%llx]\n", offsets.job_gauge, mem_ida_addr(mem, offsets.job_gauge));
    printf("target_manager: %llx [%llx]\n", offsets.target_manager, mem_ida_addr(mem, offsets.target_manager));

    offsets.fn_get_adjusted_icon = mem_scan(mem, "E8 ?? ?? ?? ?? 8B F8 3B DF", MemorySectionType::Text);

    printf("fn_get_adjusted_icon: %llx [%llx]\n", offsets.fn_get_adjusted_icon, mem_ida_addr(mem, offsets.fn_get_adjusted_icon));

    return offsets;
}

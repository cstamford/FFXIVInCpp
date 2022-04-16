#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdio>
#include <string>
#include <vector>

#include <Offsets/Memory.hpp>

uintptr_t mem_scan(const MemoryData* mem, const char* signature, const MemorySectionType::Enum section)
{
    std::string str = signature;
    str.erase(std::remove(std::begin(str), std::end(str), ' '), std::end(str));

    std::vector<bool> mask;
    std::vector<uint8_t> pattern;
    mask.reserve(str.size() / 2);
    pattern.reserve(str.size() / 2);

    for (int i = 0; i < str.size(); i += 2)
    {
        const bool masked_off = memcmp(str.data() + i, "??", 2) == 0;
        mask.emplace_back(!masked_off);

        if (!masked_off)
        {
            char num_str[3];
            memcpy(num_str, str.data() + i, 2);
            num_str[2] = '\0';
            pattern.emplace_back((uint8_t)strtol(num_str, nullptr, 16));
        }
        else
        {
            pattern.emplace_back(-1);
        }
    }

    const uintptr_t section_offset = mem->sections[section].offset;
    const uintptr_t section_size = mem->sections[section].size;

    uintptr_t addr = 0;

    for (uintptr_t offset = 0; offset < section_size - pattern.size();)
    {
        bool found = true;

        for (int i = 0; i < pattern.size(); ++i)
        {
            const uint8_t data = mem_read<uint8_t>(mem->base_addr + section_offset + offset, i);
            if (mask[i] && data != pattern[i])
            {
                offset += i + 1;
                found = false;
                break;
            }
        }

        if (found)
        {
            addr = mem->base_addr + section_offset + offset;
            break;
        }
    }

    if (addr)
    {
        // If the pattern starts on a call, we assume the user wants to locate the call site instead.
        const uint8_t inst = mem_read<uint8_t>(addr);
        if (inst == 0xE8 || inst == 0xE9) // call
        {
            return addr + mem_read<int32_t>(addr, 1) + 5; // Relative call
        }
    }

    return addr;
}

uintptr_t mem_lookup_data(const MemoryData* mem, const uintptr_t text_addr)
{
    // We're going to step through memory from this address, looking for a relative mov...
    // We read four bytes, we check if these four bytes fall into any of our data sections (relative to the next instruction)
    // If we do, we've resolved.

    const MemorySection& data = mem->sections[MemorySectionType::Data];
    const MemorySection& rdata = mem->sections[MemorySectionType::RData];

    static constexpr uintptr_t MAX_INST_LEN = 1024;
    for (uintptr_t i = 0; i < MAX_INST_LEN; ++i)
    {
        // This is the value of the assembly...
        const uintptr_t offset = mem_read<int32_t>(text_addr + i);

        // The resolved address is the full address, plus the number of bytes we have skipped, plus 4 (RIP relative load; next inst), plus the offset
        const uintptr_t resolved_addr = text_addr + i + 4 + offset;

        // Now check if the offset (resolved address minus base address) falls in either data or rdata.
        // To find this offset in IDA, it's the base address + the offset_addr; the base is usually 0x140000000
        if (const uintptr_t offset_addr = resolved_addr - mem->base_addr;
            (offset_addr >= data.offset && offset_addr < data.offset + data.size) ||
            (offset_addr >= rdata.offset && offset_addr < rdata.offset + rdata.size))
        {
            return resolved_addr;
        }
    }

    return 0;
}

uintptr_t mem_ida_addr(const MemoryData* mem, const uintptr_t addr)
{
    return addr - mem->base_addr + 0x140000000;
}

MemoryData mem_init()
{
    MemoryData data;

    const uintptr_t base_addr = (uintptr_t)GetModuleHandle(NULL);
    const uintptr_t nt_header_offset = mem_read<int32_t>(base_addr, 0x3C);
    const uintptr_t section_start_offset = nt_header_offset + 0x108;

    const int16_t num_headers = mem_read<int16_t>(base_addr + nt_header_offset, 0x6);

    for (int i = 0; i < num_headers; ++i)
    {
        const uintptr_t section_offset = section_start_offset + i * 40;
        const int64_t section_type = mem_read<int64_t>(base_addr, section_offset);

        MemorySectionType::Enum type = MemorySectionType::EnumCount;

        if (section_type == 0x747865742E /* .text */)           type = MemorySectionType::Text;
        else if (section_type == 0x617461642E /* .data */)      type = MemorySectionType::Data;
        else if (section_type == 0x61746164722E /* .rdata */)   type = MemorySectionType::RData;

        if (type != MemorySectionType::EnumCount)
        {
            data.sections[type].size = mem_read<int32_t>(base_addr, section_offset + 8);
            data.sections[type].offset = mem_read<int32_t>(base_addr, section_offset + 12);
        }
    }

    data.base_addr = base_addr;

    printf("Base address: %llx\n", data.base_addr);
    printf(".text: %llx -> %llx\n", data.sections[MemorySectionType::Text].offset, data.sections[MemorySectionType::Text].offset + data.sections[MemorySectionType::Text].size);
    printf(".data: %llx -> %llx\n", data.sections[MemorySectionType::Data].offset, data.sections[MemorySectionType::Data].offset + data.sections[MemorySectionType::Data].size);
    printf(".rdata: %llx -> %llx\n", data.sections[MemorySectionType::RData].offset, data.sections[MemorySectionType::RData].offset + data.sections[MemorySectionType::RData].size);

    return data;
}

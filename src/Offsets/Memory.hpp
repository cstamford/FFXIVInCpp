#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

struct MemorySectionType
{
    enum Enum
    {
        Text,
        Data,
        RData,
        EnumCount
    };
};

struct MemorySection
{
    uintptr_t offset = -1;
    uintptr_t size = -1;
};

struct MemoryData
{
    uintptr_t base_addr;
    MemorySection sections[3];
};

MemoryData mem_init();

uintptr_t mem_scan(const MemoryData* mem, const char* signature, const MemorySectionType::Enum section);
uintptr_t mem_lookup_data(const MemoryData* mem, const uintptr_t text_addr);
uintptr_t mem_ida_addr(const MemoryData* mem, const uintptr_t addr);

template <typename T>
T mem_read(uintptr_t address, uintptr_t offset = 0)
{
    static_assert(std::is_trivially_copyable<T>::value);
    T data;
    memcpy(&data, (void*)(address + offset), sizeof(T));
    return data;
}

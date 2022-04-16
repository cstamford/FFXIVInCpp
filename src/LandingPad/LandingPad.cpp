#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>

#include <Offsets/Memory.hpp>
#include <Offsets/Offsets.hpp>

MemoryData g_mem_data;
OffsetData g_offsets_data;

BOOL APIENTRY DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		printf("LandingPad attached.\n");

		g_mem_data = mem_init();
		g_offsets_data = offsets_init(&g_mem_data);

		printf("LandingPad ready: loading Project.\n");
	}

	return TRUE;
}

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>

#include <Offsets/Memory.hpp>
#include <Offsets/Offsets.hpp>

#include <chrono>
#include <string>

MemoryData g_mem_data;
OffsetData g_offsets_data;

struct Project
{
	HMODULE handle = NULL;

	using Init = void(*)(MemoryData* mem, OffsetData* offsets);
	using Update = void(*)(Framework* framework, const float dt);
	using Free = void(*)();

	Init init = nullptr;
	Update update = nullptr;
	Free free = nullptr;
} g_project;

std::string g_path_to_landing_pad;

bool (*update_hook_original)(Framework* instance);
bool update_hook(Framework* instance);

void hooks_init()
{
	printf("\nHooking functions.\n");

	void* update = (void*)(*g_offsets_data.framework)->vtable->fn_update;
	mem_hook("framework::update()", update, &update_hook, (void**)&update_hook_original);
}

void project_load()
{
	std::string path_to_proj = g_path_to_landing_pad.substr(0, g_path_to_landing_pad.rfind('\\') + 1) + "Project.dll";
	printf("\nLoading project %s.\n", path_to_proj.c_str());

	g_project.handle = LoadLibrary(path_to_proj.c_str());

	if (g_project.handle == NULL)
	{
		printf("Failed to load project with error %d!\n", GetLastError());
		return;
	}

	g_project.init = (Project::Init)GetProcAddress(g_project.handle, "project_init");
	g_project.update = (Project::Update)GetProcAddress(g_project.handle, "project_update");
	g_project.free = (Project::Free)GetProcAddress(g_project.handle, "project_free");

	printf("init: %llx\n", (uintptr_t)g_project.init);
	printf("update: %llx\n", (uintptr_t)g_project.update);
	printf("free: %llx\n", (uintptr_t)g_project.free);

	if (g_project.init != nullptr)
	{
		g_project.init(&g_mem_data, &g_offsets_data);
	}
}

void project_unload()
{
	printf("\nUnloading project.\n");

	if (g_project.free != nullptr)
	{
		g_project.free();
	}

	FreeLibrary(g_project.handle);
	g_project = {};
}

BOOL APIENTRY DllMain(HINSTANCE module, DWORD reason, LPVOID)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
#ifndef NDEBUG
		//while (!IsDebuggerPresent())
		{
			Sleep(100);
		}

		AllocConsole();
		freopen("CONOUT$", "w", stdout);
#endif

		static char path[MAX_PATH];
		GetModuleFileName(module, path, sizeof(path));
		g_path_to_landing_pad = path;
		printf("%s injected.\n", g_path_to_landing_pad.c_str());

		g_mem_data = mem_init();
		g_offsets_data = offsets_init(&g_mem_data);

		hooks_init();
		project_load();
	}

	return TRUE;
}

bool update_hook(Framework* instance)
{
	static std::chrono::high_resolution_clock::time_point last;
	const auto now = std::chrono::high_resolution_clock::now();
	const float dt = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last).count() / 1000.0f / 1000.0f / 1000.0f;
	last = now;

	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F11) && g_project.handle != NULL)
	{
		project_unload();
	}

	if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_F12) && g_project.handle == NULL)
	{
		project_load();
	}

	if (g_project.update != nullptr)
	{
		g_project.update(instance, dt);
	}

	return update_hook_original(instance);
}

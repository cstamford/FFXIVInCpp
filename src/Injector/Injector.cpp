#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <psapi.h>
#include <TlHelp32.h>

#include <cstdio>

#define CHECK_ERROR(x) do { if (!(x)) { printf("Error %d: Generic error (%s)!\n", GetLastError(), #x); return 1; } } while (0)

int grant_process_permissions()
{
    HANDLE token_handle = nullptr;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token_handle))
    {
        return GetLastError();
    }

    LUID luid;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
    {
        CloseHandle(token_handle);
        return GetLastError();
    }

    TOKEN_PRIVILEGES privs;
    privs.PrivilegeCount = 1;
    privs.Privileges[0].Luid = luid;
    privs.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(token_handle, FALSE, &privs, sizeof(privs), NULL, NULL))
    {
        CloseHandle(token_handle);
        return GetLastError();
    }

    return 0;
}

DWORD get_pid(const char* process)
{
    HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    PROCESSENTRY32 proc_entry;
    proc_entry.dwSize = sizeof(proc_entry);

    DWORD pid = -1;

    while (Process32Next(handle, &proc_entry))
    {
        if (!strcmp(proc_entry.szExeFile, process))
        {
            pid = proc_entry.th32ProcessID;
            break;
        }
    }

    return pid;
}

HMODULE get_dll_handle(const char* dll_name, const HANDLE process)
{
    HMODULE modules[1024];
    DWORD bytes_needed;

    if (EnumProcessModules(process, modules, sizeof(modules), &bytes_needed))
    {
        for (int i = 0; i < bytes_needed / sizeof(HMODULE); i++)
        {
            char module_name[MAX_PATH];

            if (GetModuleFileNameEx(process, modules[i], module_name, sizeof(module_name)))
            {
                if (strcmp(module_name, dll_name) == 0) return modules[i];
            }
        }
    }

    return nullptr;
}

int inject(LPTHREAD_START_ROUTINE func, HANDLE process, void* data, const int data_len)
{
    LPVOID remote_memory_addr = VirtualAllocEx(process, NULL, data_len, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    CHECK_ERROR(remote_memory_addr);

    CHECK_ERROR(WriteProcessMemory(process, remote_memory_addr, data, data_len, NULL));

    HANDLE remote_thread_handle = CreateRemoteThread(process, 0, 0, (LPTHREAD_START_ROUTINE)func, remote_memory_addr, 0, 0);
    CHECK_ERROR(remote_thread_handle);

    WaitForSingleObject(remote_thread_handle, INFINITE);
    CloseHandle(remote_thread_handle);

    CHECK_ERROR(VirtualFreeEx(process, remote_memory_addr, 0, MEM_RELEASE));

    return 0;
}

int main()
{
    if (const int ret = grant_process_permissions(); ret != 0)
    {
        printf("grant_process_permissions() failed: error %d!\n", ret);
        return 1;
    }

    char dll_path[MAX_PATH];
    GetFullPathNameA("LandingPad.dll", MAX_PATH, dll_path, NULL);

    const DWORD pid = get_pid("ffxiv_dx11.exe");

    if (pid == -1)
    {
        printf("get_pid() failed!\n");
        return 1;
    }

    const DWORD access = PROCESS_CREATE_THREAD |
        PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ |
        PROCESS_VM_WRITE |
        PROCESS_VM_OPERATION;

    HANDLE process_handle = OpenProcess(access, FALSE, pid);

    if (process_handle == NULL)
    {
        printf("OpenProcess() failed: error %d!\n", GetLastError());
        return 1;
    }

    if (HMODULE dll = get_dll_handle(dll_path, process_handle))
    {
        printf("Already injected into this process; bailing.\n");
        return 1;
    }

    inject((LPTHREAD_START_ROUTINE)LoadLibraryA, process_handle, dll_path, sizeof(dll_path));
    CloseHandle(process_handle);

    return 0;
}
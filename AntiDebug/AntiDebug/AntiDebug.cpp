#include "AntiDebug.h"
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

#include "StrEncode.h"


//#define lprintf(format, ...) printf("[%s:%d]:" format "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define lprintf(...) 


typedef BOOL(__stdcall* pType_CheckRemoteDebuggerPresent)(HANDLE hProcess, PBOOL pbDebuggerPresent);
typedef NTSTATUS(WINAPI* pType_ZwQueryInformationProcess)(
    _In_      HANDLE           ProcessHandle,
    _In_      UINT             ProcessInformationClass, //PROCESSINFOCLASS
    _Out_     PVOID            ProcessInformation,
    _In_      ULONG            ProcessInformationLength,
    _Out_opt_ PULONG           ReturnLength);



//取进程PID 成功返回PID 失败返回0
static inline DWORD FindProcessPid(LPCSTR ProcessName)
{
    DWORD PID = 0;
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return -1;
    }

    char szUpperProcessName[1024];
    strcpy_s(szUpperProcessName, sizeof(szUpperProcessName), ProcessName);
    _strupr_s(szUpperProcessName, sizeof(szUpperProcessName));

    char szUpperExeFile[1024];
    do
    {
        strcpy_s(szUpperExeFile, sizeof(szUpperExeFile), pe32.szExeFile);
        _strupr_s(szUpperExeFile, sizeof(szUpperExeFile));
        if (!strcmp(szUpperProcessName, szUpperExeFile))
        {
            PID = pe32.th32ProcessID;
            break;
        }

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return PID;
}

static inline bool mIsDebuggerPresent()
{
    ////check softbreak
    //if (*(unsigned char*)IsDebuggerPresent == 0xcc) return true;
    ////check hook
    //if (*(unsigned char*)IsDebuggerPresent != 0x64) return true;
    int asm_ret = 0;
    __asm
    {
        mov eax, fs: [30h]
        mov eax, [eax + 2]
        and eax, 0x000000ff //AL = PEB.BeingDebugged    //eax &= 0xFF 取al
        mov asm_ret, eax
    }
    lprintf(_c("asm_ret[%d]"), asm_ret);
    return asm_ret != 0;
}

bool FD_IsDebuggerPresent()
{
    return mIsDebuggerPresent();
}

bool FD_PEB_BeingDebuggedFlag()
{
    return mIsDebuggerPresent();
}

bool FD_PEB_NtGlobalFlags()
{
    int asm_ret = 0;
    __asm
    {
        mov eax, fs: [30h]
        mov eax, [eax + 68h]
        and eax, 0x70
        mov asm_ret, eax
    }
    //_HEAP_ENABLE_TAIL_CHECK  (0x10)
    //_HEAP_ENABLE_FREE_CHECK  (0x20)
    //_HEAP_VALIDATE_PARAMETERS(0x40)
    lprintf(_c("asm_ret[%d]"), asm_ret);
    return asm_ret != 0;
}

bool FD_CheckRemoteDebuggerPresent()
{
    HMODULE hKernel32Module = GetModuleHandleA(_c("kernel32.dll"));
    if (hKernel32Module == INVALID_HANDLE_VALUE || hKernel32Module == NULL) return false;
    pType_CheckRemoteDebuggerPresent fn_CheckRemoteDebuggerPresent = (pType_CheckRemoteDebuggerPresent)GetProcAddress(hKernel32Module, _c("CheckRemoteDebuggerPresent"));
    if (!fn_CheckRemoteDebuggerPresent) return false;
    BOOL isDebuggerPresent = FALSE;
    fn_CheckRemoteDebuggerPresent((HANDLE)-1, &isDebuggerPresent);
    lprintf(_c("isDebuggerPresent[%d]"), isDebuggerPresent);
    return isDebuggerPresent;
}

bool FD_NtQueryInfoProc_DbgPort()
{
    HMODULE hNtdllModule = GetModuleHandleA(_c("ntdll.dll"));
    if (!hNtdllModule) return false;
    pType_ZwQueryInformationProcess ZwQueryInformationProcess = (pType_ZwQueryInformationProcess)GetProcAddress(hNtdllModule, _c("ZwQueryInformationProcess"));
    if (ZwQueryInformationProcess == NULL) return false;
        
    DWORD dwDebuggerState = 0;
    if (ZwQueryInformationProcess(GetCurrentProcess(), 0x07, &dwDebuggerState, sizeof(DWORD), NULL) != 0)
    {
        return false;
    }
    //进程处于被调试状态时，ProcessDebugPort = 0xffffffff
    lprintf(_c("dwDebuggerState[%d]"), dwDebuggerState);
    return dwDebuggerState != 0;
}

bool FD_NtQueryInfoProc_DbgObjHandle()
{
    HMODULE hNtdllModule = GetModuleHandleA(_c("ntdll.dll"));
    if (!hNtdllModule) return false;
    pType_ZwQueryInformationProcess ZwQueryInformationProcess = (pType_ZwQueryInformationProcess)GetProcAddress(hNtdllModule, _c("ZwQueryInformationProcess"));
    if (ZwQueryInformationProcess == NULL) return false;

    DWORD dwDebuggerState = 0;
    if (ZwQueryInformationProcess(GetCurrentProcess(), 0x1E, &dwDebuggerState, sizeof(DWORD), NULL) != 0)
    {
        return false;
    }
    lprintf(_c("dwDebuggerState[%d]"), dwDebuggerState);
    return dwDebuggerState != 0;
}

bool FD_NtQueryInfoProc_DbgFlags()
{
    HMODULE hNtdllModule = GetModuleHandleA(_c("ntdll.dll"));
    if (!hNtdllModule) return false;
    pType_ZwQueryInformationProcess ZwQueryInformationProcess = (pType_ZwQueryInformationProcess)GetProcAddress(hNtdllModule, _c("ZwQueryInformationProcess"));
    if (ZwQueryInformationProcess == NULL) return false;

    DWORD dwDebuggerState = 0;
    if (ZwQueryInformationProcess(GetCurrentProcess(), 0x1F, &dwDebuggerState, sizeof(DWORD), NULL) != 0)
    {
        return false;
    }
    lprintf(_c("dwDebuggerState[%d]"), dwDebuggerState);
    return dwDebuggerState == 0;
}

bool FD_Find_Debugger_Window()
{  
    HWND hWnd;
    //ollyice
    hWnd = FindWindow(NULL, _c("吾愛破解"));
    if (hWnd != NULL) return true;
    //ollydbg v1.1
    hWnd = FindWindow(_c("icu_dbg"), NULL);
    if (hWnd != NULL) return true;
    //ollyice pe--diy
    hWnd = FindWindow(_c("pe--diy"), NULL);
    if (hWnd != NULL) return true;
    //ollydbg ?-°?
    hWnd = FindWindow(_c("ollydbg"), NULL);
    if (hWnd != NULL) return true;
    //ollydbg ?-°?
    hWnd = FindWindow(_c("odbydyk"), NULL);
    if (hWnd != NULL) return true;
    //windbg
    hWnd = FindWindow(_c("WinDbgFrameClass"), NULL);
    if (hWnd != NULL) return true;
    //dede3.50
    hWnd = FindWindow(_c("TDeDeMainForm"), NULL);
    if (hWnd != NULL) return true;
    //IDA5.20
    hWnd = FindWindow(_c("TIdaWindow"), NULL);
    if (hWnd != NULL) return true;
    //others
    hWnd = FindWindow(_c("TESTDBG"), NULL);
    if (hWnd != NULL) return true;
    hWnd = FindWindow(_c("kk1"), NULL);
    if (hWnd != NULL) return true;
    hWnd = FindWindow(_c("Eew75"), NULL);
    if (hWnd != NULL) return true;
    hWnd = FindWindow(_c("Shadow"), NULL);
    if (hWnd != NULL)    return true;
    
    //PEiD v0.94  hWnd=FindWindow(NULL,"PEiD v0.94");  if (hWnd!=NULL) return true; 
    //RegMON    hWnd=FindWindow(NULL,"Registry Monitor - Sysinternals: www.sysinternals.com");    if (hWnd!=NULL)    return true;  
    //File Monitor    hWnd=FindWindow(NULL,"File Monitor - Sysinternals: www.sysinternals.com");    if (hWnd!=NULL)    return true; 
    //Import Rec v1.6    hWnd=FindWindow(NULL,"Import REConstructor v1.6 FINAL (C) 2001-2003 MackT/uCF");    if (hWnd!=NULL)    return true;  return false;

    return false;
}

bool FD_Find_Debugger_Process()
{
    bool isDebuggerPresent = false;
    PROCESSENTRY32 pe32;
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) return false;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return false;
    }
    char szUpperExeFile[1024];
    do
    {
        strcpy_s(szUpperExeFile, sizeof(szUpperExeFile), pe32.szExeFile);
        _strupr_s(szUpperExeFile, sizeof(szUpperExeFile));
        if (strcmp(szUpperExeFile, _c("OLLYICE.EXE")) == 0)     { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("IDAG.EXE")) == 0)        { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("IDA.EXE")) == 0)         { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("OLLYDBG.EXE")) == 0)     { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("PEID.EXE")) == 0)        { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("SOFTICE.EXE")) == 0)     { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("LORDPE.EXE")) == 0)      { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("IMPORTREC.EXE")) == 0)   { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("W32DSM89.EXE")) == 0)    { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("WINDBG.EXE")) == 0) { isDebuggerPresent = true; break; }
        if (strcmp(szUpperExeFile, _c("X32DBG.EXE")) == 0) { isDebuggerPresent = true; break; }

    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
    lprintf(_c("isDebuggerPresent[%d]"), isDebuggerPresent);
    return isDebuggerPresent;
}

bool FD_Exception_Closehandle()
{
    //要开启eha
    __try
    {
        CloseHandle(HANDLE(0xF0001234));
        //CloseHandle(HANDLE(-1));
        return false;
    }
    __except (1)
    {
        return true;
    }
}

bool FD_Exception_Int3()
{
    __asm
    {
        push   offset exception_handler // set exception handler
        push  dword ptr fs : [0h]
        mov    dword ptr fs : [0h] , esp
        xor eax, eax; reset EAX invoke int3
        int    3h
        pop    dword ptr fs : [0h]      // restore exception handler
        add   esp, 4

        test   eax, eax; check the flag
        je    rt_label
        jmp    rf_label

        exception_handler :
        mov   eax, dword ptr[esp + 0xc]             // EAX = ContextRecord
        mov    dword ptr[eax + 0xb0], 0xffffffff    // set flag(ContextRecord.EAX)
        inc   dword ptr[eax + 0xb8]                 // set ContextRecord.EIP
        xor eax, eax
        retn

    rt_label :
        xor eax, eax
        inc eax
        mov esp, ebp
        pop ebp
        retn
    rf_label :
        xor eax, eax
        mov esp, ebp
        pop ebp
        retn
    }
}
bool FD_INT_2d()
{
    __try
    {
        __asm
        {
            int 2dh
            inc eax //any opcode of singlebyte.
            // or u can put some junkcode, "0xc8"..."0xc2"..."0xe8"..."0xe9"
        }
        return true;
    }
    __except (1)
    {
        return false;
    }
}

#include <iostream>
#include <stdio.h>
#include <windows.h>
#include "AntiDebug.h"

#include "StrEncode.h"

int main()
{
    std::cout << _c("FD_IsDebuggerPresent()             ") << FD_IsDebuggerPresent() << std::endl;
    std::cout << _c("FD_PEB_BeingDebuggedFlag()         ") << FD_PEB_BeingDebuggedFlag() << std::endl;
    std::cout << _c("FD_PEB_NtGlobalFlags()             ") << FD_PEB_NtGlobalFlags() << std::endl;
    std::cout << _c("FD_CheckRemoteDebuggerPresent()    ") << FD_CheckRemoteDebuggerPresent() << std::endl;
    std::cout << _c("FD_NtQueryInfoProc_DbgPort()       ") << FD_NtQueryInfoProc_DbgPort() << std::endl;
    std::cout << _c("FD_NtQueryInfoProc_DbgObjHandle()  ") << FD_NtQueryInfoProc_DbgObjHandle() << std::endl;
    std::cout << _c("FD_NtQueryInfoProc_DbgFlags()      ") << FD_NtQueryInfoProc_DbgFlags() << std::endl;
    std::cout << _c("FD_Find_Debugger_Window()          ") << FD_Find_Debugger_Window() << std::endl;
    std::cout << _c("FD_Find_Debugger_Process()         ") << FD_Find_Debugger_Process() << std::endl;
    std::cout << _c("FD_Exception_Closehandle()         ") << FD_Exception_Closehandle() << std::endl;
    std::cout << _c("FD_Exception_Int3()                ") << FD_Exception_Int3() << std::endl;
    std::cout << _c("FD_INT_2d()                        ") << FD_INT_2d()                        << std::endl;

    //for (size_t i = 0; i < 111111111; i++)
    //{
    //
    //    std::cout << _c("FD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2dFD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //    std::cout << _c("FD_INT_2d()") << std::endl;
    //    std::wcout << _cw(L"FD_INT_2d()") << std::endl;
    //}

    std::cout << "Hello World!\n";
    std::wcout << L"Hello World!\n";
    system("pause");
    return 0;
}

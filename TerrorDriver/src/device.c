#pragma warning (disable : 4047 4024 4100)

#include "device.h"
#include "dbgprint.h"

#ifdef _WIN64
// For x64 builds — most Windows 10/11 versions use this offset
#define PsGetProcessPeb(Process) ((PPEB)(*(PVOID *)((PUCHAR)(Process) + 0x550)))
#else
// For x86 builds — typical offset for 32-bit Windows
#define PsGetProcessPeb(Process) ((PPEB)(*(PVOID *)((PUCHAR)(Process) + 0x1b0)))
#endif

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	DebugPrint("Communication established!\n");

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	DebugPrint("Communication closed!\n");

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS IOControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_UNSUCCESSFUL;

	ULONG ByteIo = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_UNMAP_NTDLL)
	{
		
		DWORD pid = *(DWORD*)Irp->AssociatedIrp.SystemBuffer;

		DebugPrint("pid passed: %u\n", pid);
		

		PVOID ntdll = GetNtDll(pid);


		DebugPrint("ntdll address: %p", ntdll);


		PEPROCESS pEProcess = NULL;
		HANDLE handle = NULL;

		status = PsLookupProcessByProcessId((HANDLE)pid, &pEProcess);
		if (!NT_SUCCESS(status)) 
		{
			DbgPrint("[-] PsLookupProcessByProcessId failed: 0x%X\n", status);
			return status;
		}

		status = ObOpenObjectByPointer(pEProcess, OBJ_KERNEL_HANDLE, NULL, 0x0008, *PsProcessType, KernelMode, &handle); //PROCESS_VM_OPERATION

		if (!NT_SUCCESS(status)) 
		{
			DbgPrint("[-] ObOpenObjectByPointer failed: 0x%X\n", status);
			ObDereferenceObject(pEProcess);
			return status;
		}

		status = ZwUnmapViewOfSection(handle, ntdll);

		if (NT_SUCCESS(status)) 
		{
			DbgPrint("[+] ZwUnmapViewOfSection succeeded for %p\n", ntdll);
		}
		else 
		{
			DbgPrint("[-] ZwUnmapViewOfSection FAILED for %p -> Status: 0x%X\n", ntdll, status);
		}

		ZwClose(handle);
		status = STATUS_SUCCESS;
	}



	



	else
	{
		DebugPrint("Unknown control code\n");

		ByteIo = 0;
	}


	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}




PVOID GetNtDll(DWORD pid)
{

	
	PVOID ntdllBase = NULL;
	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)pid, &targetProcess);

	if (!NT_SUCCESS(status)) 
	{
		DebugPrint("Failed to get EPROCESS\n");
		return NULL;
	}

	KAPC_STATE apc;
	KeStackAttachProcess(targetProcess, &apc);

	PPEB peb = PsGetProcessPeb(targetProcess);
	if (peb && MmIsAddressValid(peb)) 
	{
		PPEB_LDR_DATA ldr = peb->Ldr;
		if (ldr && MmIsAddressValid(ldr)) 
		{
			PLIST_ENTRY head = &ldr->InLoadOrderModuleList;
			if (MmIsAddressValid(head)) 
			{
				PLIST_ENTRY curr = head->Flink;

				while (curr && curr != head && MmIsAddressValid(curr)) 
				{
					PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(curr, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
					if (MmIsAddressValid(entry) && MmIsAddressValid(entry->BaseDllName.Buffer)) 
					{
						if (_wcsicmp(entry->BaseDllName.Buffer, L"ntdll.dll") == 0) 
						{
							ntdllBase = entry->DllBase;
							break;
						}
					}

					curr = curr->Flink;
				}
			}
		}
	}

	KeUnstackDetachProcess(&apc);
	ObDereferenceObject(targetProcess);

	return ntdllBase;

}
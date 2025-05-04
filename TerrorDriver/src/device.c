#pragma warning (disable : 4047 4024 4100)

#include "device.h"
#include "dbgprint.h"

NTSYSAPI NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS FromProcess, PVOID FromAddress, PEPROCESS ToProcess, PVOID ToAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);

NTSYSCALLAPI NTSTATUS NTAPI ZwGetNextThread(_In_ HANDLE ProcessHandle, _In_ HANDLE ThreadHandle, _In_ ACCESS_MASK DesiredAccess, _In_ ULONG HandleAttributes, _In_ ULONG Flags, _Out_ PHANDLE NewThreadHandle);

#ifdef _WIN64
// For x64 builds
#define PsGetProcessPeb(Process) ((PPEB)(*(PVOID *)((PUCHAR)(Process) + 0x550)))
#else
// For x86 builds 
#define PsGetProcessPeb(Process) ((PPEB)(*(PVOID *)((PUCHAR)(Process) + 0x1b0)))
#endif

#ifdef _WIN64
// For x64 builds 
#define PsGetThreadTeb(Thread) ((*(PVOID *)((PUCHAR)(Thread) + 0x030)))
#else
// For x86 builds
#define PsGetThreadTeb(Thread) ((*(PVOID *)((PUCHAR)(Thread) + 0x0e0)))
#endif

//#define THREAD_GET_CONTEXT 0x0008
#define THREAD_QUERY_INFORMATION 0x0040

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

	do
	{
		if (ControlCode == IO_UNMAP_NTDLL)
		{

			DWORD pid = *(DWORD*)Irp->AssociatedIrp.SystemBuffer;

			DebugPrint("Control code 0x111 was passed with pid: %u\n", pid);

			PVOID ntdll = GetNtDll(pid);

			DebugPrint("ntdll address: %p\n", ntdll);

			PEPROCESS pEProcess = NULL;
			HANDLE handle = NULL;

			status = PsLookupProcessByProcessId((HANDLE)pid, &pEProcess);
			if (!NT_SUCCESS(status))
			{
				DebugPrint("[-] PsLookupProcessByProcessId failed: 0x%X\n", status);
				break;
			}

			status = ObOpenObjectByPointer(pEProcess, OBJ_KERNEL_HANDLE, NULL, 0x0008, *PsProcessType, KernelMode, &handle); //PROCESS_VM_OPERATION

			if (!NT_SUCCESS(status))
			{
				DebugPrint("[-] ObOpenObjectByPointer failed: 0x%X\n", status);
				ObDereferenceObject(pEProcess);
				break;
			}

			status = ZwUnmapViewOfSection(handle, ntdll);

			if (NT_SUCCESS(status))
			{
				DebugPrint("[+] ZwUnmapViewOfSection succeeded for %p\n", ntdll);
			}
			else
			{
				DebugPrint("[-] ZwUnmapViewOfSection FAILED for %p -> Status: 0x%X\n", ntdll, status);
			}

			ZwClose(handle);
			status = STATUS_SUCCESS;
		}
		else if (ControlCode == IO_CORRUPT_PEB)
		{
			DWORD pid = *(DWORD*)Irp->AssociatedIrp.SystemBuffer;

			DebugPrint("Control code 0x222 was passed with pid: %u\n", pid);


			status = PsLookupProcessByProcessId((HANDLE)pid, &targetProcess);

			if (!NT_SUCCESS(status))
			{
				DebugPrint("Failed to get EPROCESS\n");
				break;
			}

			PPEB peb = PsGetProcessPeb(targetProcess);

			PEB data = { 0 };
			SIZE_T bytes = 0;

			status = MmCopyVirtualMemory(PsGetCurrentProcess(), &data, targetProcess, peb, sizeof(PEB), KernelMode, &bytes);

			if (NT_SUCCESS(status))
			{
				DebugPrint("MmCopyVirtualMemory succeeded -> 0x%X\n", peb);
			}
			else
			{
				DebugPrint("MmCopyVirtualMemory failed 0x%X\n", status);
				break;
			}
			status = STATUS_SUCCESS;


		}
		//else if (ControlCode == IO_CORRUPT_STACK)
		//{
		//	DWORD pid = *(DWORD*)Irp->AssociatedIrp.SystemBuffer;

		//	DebugPrint("Control code 0x333 was passed with pid: %u\n", pid);

		//	status = PsLookupProcessByProcessId((HANDLE)pid, &targetProcess);

		//	if (!NT_SUCCESS(status))
		//	{
		//		DebugPrint("Failed to get EPROCESS\n");
		//		break;
		//	}

		//	HANDLE threadHandle = NULL;
		//	HANDLE processHandle = NULL;


		//	status = ObOpenObjectByPointer(targetProcess, OBJ_KERNEL_HANDLE, NULL, 0x0008, *PsProcessType, KernelMode, &processHandle);

		//	if (!NT_SUCCESS(status))
		//	{
		//		DebugPrint("ObOpenObjectByPointer failed: 0x%X\n", status);
		//		return status;
		//	}

		//	status = ZwGetNextThread(processHandle, NULL, THREAD_QUERY_LIMITED_INFORMATION | THREAD_GET_CONTEXT, 0, 0, &threadHandle);

		//	if (!NT_SUCCESS(status))
		//	{
		//		DebugPrint("ZwGetNextThread failed: 0x%X", status);
		//	}


		//	PETHREAD thread = NULL;


		//	status = ObReferenceObjectByHandle(threadHandle, 0x0040, *PsThreadType, KernelMode, (PVOID*)&thread, NULL);


		//	if (!NT_SUCCESS(status))
		//	{
		//		DebugPrint("ObReferenceObjectByHandle failed: 0x%X", status);
		//	}

		//	PVOID teb = PsGetThreadTeb(thread);

		//	if (teb == NULL)
		//	{
		//		DebugPrint("teb is NULL\n");
		//	}

		//	//PVOID stackBase = *(PVOID*)((PUCHAR)teb + 0x8);   // TEB->StackBase
		//	//PVOID stackLimit = *(PVOID*)((PUCHAR)teb + 0x10);  // TEB->StackLimit


		//	KAPC_STATE apc;
		//	KeStackAttachProcess(targetProcess, &apc);

		//	NT_TIB* tib = (NT_TIB*)teb;
		//	SIZE_T bytes = 0;

		//	status = MmCopyVirtualMemory(targetProcess, teb, PsGetCurrentProcess(), &tib, sizeof(NT_TIB), KernelMode, &bytes);

		//	if (!NT_SUCCESS(status))
		//	{
		//		DebugPrint("MmCopyVirtualMemory failed (tib) -> 0x%X", status);
		//		break;
		//	}

		//	DebugPrint("MmCopyVirtualMemory status: 0x%X, bytesCopied: %llu\n", status, bytes);



		//	PVOID stackBase = tib->StackBase;
		//	PVOID stackLimit = tib->StackLimit;

		//	SIZE_T stackSize = (SIZE_T)stackLimit - (SIZE_T)stackBase;

		//	DebugPrint("TEB: %p  Stack: %p - %p  Stack size: %zu\n", teb, stackLimit, stackBase, stackSize);

		//	PVOID buffer = ExAllocatePoolWithTag(NonPagedPoolNx, stackSize, 'pool');
		//	SIZE_T bytes = 0;

		//	if (!buffer)
		//	{
		//	DebugPrint("Failed to allocate pool\n");
		//	  status = STATUS_UNSUCCESSFUL;
		//	  break;
		//	}

		//	memset(buffer, 0, stackSize);

		//	status = MmCopyVirtualMemory(PsGetCurrentProcess(), &buffer, targetProcess, stackBase, stackSize, KernelMode, &bytes);

		//	if (!NT_SUCCESS(status))
		//	{
		//		DebugPrint("MmCopyVirtualMemory failed 0x%X\n", status);
		//	}

		//	ExFreePool(buffer);
		//	ObDereferenceObject(thread);


		//	KeUnstackDetachProcess(&apc);
		//	status = STATUS_SUCCESS;
		//}
		else
		{
			DebugPrint("Unknown control code\n");

			ByteIo = 0;
		}
	} while (FALSE);

	Irp->IoStatus.Status = status;
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
		return status;
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

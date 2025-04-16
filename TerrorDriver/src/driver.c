#pragma warning (disable : 4047 4024 4100)

#include "driver.h"
#include "device.h"
#include "dbgprint.h"


UNICODE_STRING dev, dos;
PDEVICE_OBJECT pDeviceObject;


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{

	UNREFERENCED_PARAMETER(pDriverObject);
	UNREFERENCED_PARAMETER(pRegistryPath);

	pDriverObject->DriverUnload = DriverUnload;

	NTSTATUS status = STATUS_SUCCESS;

	DebugPrint("Driver Initialized\n");

	RtlInitUnicodeString(&dev, L"\\Device\\Terror");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\Terror");

	IoCreateDevice(pDriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
	IoCreateSymbolicLink(&dos, &dev);

	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOControl;

	pDeviceObject->Flags |= DO_DIRECT_IO;
	pDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;


	return status;
}



NTSTATUS DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	UNREFERENCED_PARAMETER(pDriverObject);

	NTSTATUS status = STATUS_SUCCESS;

	IoDeleteSymbolicLink(&dos);
	IoDeleteDevice(pDriverObject->DeviceObject);

	DebugPrint("Driver unloaded\n");

	return status;
}



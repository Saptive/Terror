# Terror


**Terror** is a malware project inspiered by the Chineese **GanDiao.sys** rootkit driver from the early 2000's and the recent Black Lotus bootkit. The project is aimed at learning more about kenrel development along with boot- and rootkit techniques.  

## Features
- **Unmapping ntdll.dll** - The driver opens a handle to the target process (PROCESS_VM_OPERATION), and then finds the baseaddress of ntdll.dll by walking the PEB. Once found, it uses **ZwUnmapViewOfSection** to unmap it. This causes the process to crash with an access violation at the next call into ntdll.
 - **PEB corruption** - The driver attaches to the target process, and retrieves the address of the PEB. Then using MmCopyVirtualMemory, it fills it with null bytes. The process crashes at launch with an access violation.


## To be added
### Driver
- **Elevate um process to SYSTEM**
- **Stack corruption**


### Bootkit
- **Load the driver before ELAM drivers**
- **Disable DSE and PG**
- **Suport for both MBR and UEFI systems**

### Usermode executable
- **Support for any unwanted process**
- **A more reliable way to check for processes**


## Screenshots
![unmapping ntdll](https://github.com/user-attachments/assets/214ae143-ace5-43dc-b786-ebf5af87ed6f)
![PEB corruption](https://github.com/user-attachments/assets/563ab985-5909-4fcb-a870-f18522823940)
### Prerequisites
- **Visual studio with WDK installed**

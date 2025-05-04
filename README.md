# Terror


**Terror** is a malware project inspiered by the Chineese **GanDiao.sys** rootkit driver from the early 2000's and the recent Black Lotus bootkit. The project is aimed at learning more about kenrel development along with boot- and rootkit techniques.  

## Features
- **Unmapping ntdll.dll** - The driver opens a handle to the target process (PROCESS_VM_OPERATION), and then finds the baseaddress of ntdll.dll by walking the PEB. Once found, it uses **ZwUnmapViewOfSection** to unmap it. This causes the process to crash with an access violation at the next call into ntdll.
 - **PEB corruption** - The driver attaches to the target process, and retrieves the address of the PEB. Then using MmCopyVirtualMemory, it fills it with null bytes. The process crashes at launch with an access violation.
- **Elevate um process to SYSTEM privilage** - The driver elevates a target user-mode process by replacing its access token with that of the SYSTEM process (PID 4). It retrieves both EPROCESS structures and overwrites the target’s Token field, granting it SYSTEM-level privileges

## To be added
### Driver
- **Stack corruption**


### Bootkit
- **Load the driver before ELAM drivers**
- **Disable DSE and PG**
- **Suport for both MBR and UEFI systems**

### User-mode executable
- **Support for any unwanted process**
- **A more reliable way to check for processes**


## Screenshots
![unmapping ntdll](https://github.com/user-attachments/assets/214ae143-ace5-43dc-b786-ebf5af87ed6f)
![PEB corruption](https://github.com/user-attachments/assets/563ab985-5909-4fcb-a870-f18522823940)
![Token elevation](https://github.com/user-attachments/assets/569c05fd-cbe2-477a-8a69-8ac77c236f26)
### Prerequisites
- **Visual studio with WDK installed**

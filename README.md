# Terror


**Terror** is a malware project inspiered by the Chineese **GanDiao.sys** rootkit driver from the early 2000's and the recent Black Lotus bootkit. The project is aimed at learning more about kenrel development along with boot- and rootkit techniques. The project so far includes a driver and a usermode executable. 

## Features
- **Unmapping ntdll.dll** - The driver opens a handle to the target process (PROCESS_VM_OPERATION), and then finds the baseaddress of ntdll.dll by walking the PEB. Once found, it uses **ZwUnmapViewOfSection** to unmap it. This causes the process to crash with an access violation at the next call into ntdll.
 


## To be added
- **Stack corruption**
- **PEB corruption**
- **Remote thread crashing**
- **Manual mapping for the driver**
- **Custom bootloader to load the driver to avoid DSE**


## Screenshots
![unmapping ntdll](https://github.com/user-attachments/assets/214ae143-ace5-43dc-b786-ebf5af87ed6f)

### Prerequisites
- **Visual studio with WDK installed**

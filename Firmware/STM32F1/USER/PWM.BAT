SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\Java\jdk-11\bin;C:\Program Files\VanDyke Software\Clients\;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files\NVIDIA Corporation\NVIDIA NGX;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;C:\Program Files (x86)\IDM Computer Solutions\UltraEdit;C:\Program Files\IDM Computer Solutions\UltraCompare;C:\Program Files\Microsoft SQL Server\Client SDK\ODBC\110\Tools\Binn\;C:\Program Files (x86)\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files\Microsoft SQL Server\120\Tools\Binn\;C:\Program Files\Microsoft SQL Server\120\DTS\Binn\;C:\Program Files (x86)\Windows Kits\8.1\Windows Performance Toolkit\;C:\Program Files\PuTTY\;C:\Program Files (x86)\IVI Foundation\IVI\bin;C:\Program Files\IVI Foundation\IVI\bin;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin\;C:\Program Files\IVI Foundation\VISA\Win64\Bin\;C:\Program Files\Microsoft SQL Server\130\Tools\Binn\;C:\Program Files\dotnet\;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\MatlabGUI\MATLAB Runtime\v910\runtime\win64;C:\Program Files\Git\cmd;E:\Matlab_R2020\runtime\win64;E:\Matlab_R2020\bin;E:\Matlab_R2020\polyspace\bin;C:\Program Files\nodejs\;C:\ProgramData\chocolatey\bin;C:\Users\win10\scoop\shims;C:\Users\win10\AppData\Local\Programs\Python\Python37-32\Scripts\;C:\Users\win10\AppData\Local\Programs\Python\Python37-32\;C:\Users\win10\AppData\Local\Microsoft\WindowsApps;C:\Program Files\Bandizip\;C:\Users\win10\.dotnet\tools;C:\Users\win10\AppData\Local\Microsoft\WindowsApps;C:\Program Files (x86)\Nmap;C:\Program Files\JetBrains\PhpStorm 2020.3.3\bin;C:\Users\win10\AppData\Local\Programs\Microsoft VS Code\bin;C:\Program Files (x86)\Tencent\微信web开发者工具\dll;C:\Users\win10\AppData\Roaming\npm;C:\ProgramData\Anaconda3\Scripts
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=PWM
SET CPU_CLOCK=0x00B71B00
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f10x_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\system_stm32f10x.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\key.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\timer.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\sys.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\usart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\bsptime.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\im1281b.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\core_cm3.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via "..\obj\startup_stm32f10x_md._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\misc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f10x_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f10x_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f10x_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via "..\obj\stm32f10x_usart.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via "..\OBJ\PWM.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" "..\OBJ\PWM.axf" --i32combined --output "..\OBJ\PWM.hex"

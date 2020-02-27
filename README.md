# SyscallHide
Create a Run registry key with direct system calls. Inspired by [@Cneelis
](https://twitter.com/Cneelis)'s Dumpert and SharpHide.

Odzhan & [@therealwover](https://twitter.com/therealwover)'s [Donut](https://github.com/TheWover/donut) tool can be used if you don't want to touch the disk.

The tool was tested against:
* Windows Server 2016
* Windows 10 1903
* Windows 7 SP1

# Usage
```
SyscallHide.exe create C:\Windows\Temp\backdoor.exe argument1
```

# Example

```
SyscallHide.exe create C:\Windows\Temp\backdoor.exe arguement1
[>] Checking OS version details:
         [>] Operating System is Windows 10 or Server 2016, build number 18362.
         [>] Mapping version specific system calls.
         [>] SyscallHide is not running as elevated user.
```

# Bugs
For any bugs give me a shout on Twitter [@den_n1s](https://twitter.com/den_n1s) or open an issue. 

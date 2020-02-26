.code
 
; Reference: https://j00ru.vexillium.org/syscalls/nt/64/
; Windows 7 SP1 & Server 2008 R2
ZwOpenKey7SP1 proc
             mov r10, rcx
             mov eax, 0Fh
             syscall
             ret
ZwOpenKey7SP1 endp
NtSetValueKey7SP1 proc
             mov r10, rcx
             mov eax, 5Dh
             syscall
             ret
NtSetValueKey7SP1 endp
NtDeleteValueKey7SP1 proc
             mov r10, rcx
             mov eax, 0B3h
             syscall
             ret
NtDeleteValueKey7SP1 endp
NtClose7SP1 proc
             mov r10, rcx
             mov eax, 0Ch
             syscall
             ret
NtClose7SP1 endp
; Windows 10 1903 & Server 2016
 
ZwOpenKey10 proc
             mov     r10, rcx
             mov     eax, 12h
             syscall
             ret
ZwOpenKey10 endp
NtSetValueKey10 proc
              mov     r10, rcx
              mov     eax, 60h
              syscall
              ret
NtSetValueKey10 endp
NtDeleteValueKey10 proc
                 mov     r10, rcx
                 mov     eax, 0D2h
                 syscall
                 ret
NtDeleteValueKey10 endp
NtClose10 proc
       mov     r10, rcx
       mov     eax, 0Fh
       syscall
       ret
NtClose10 endp
end
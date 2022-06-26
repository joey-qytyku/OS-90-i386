STRUC TF
    ._ss:    RESD 1
    ._esp:   RESD 1
    ._eflags:RESD 1
    ._cs:    RESD 1
    ._eip:   RESD 1
    ._eax:   RESD 1
    ._ebx:   RESD 1
    ._ecx:   RESD 1
    ._edx:   RESD 1
    ._esi:   RESD 1
    ._edi:   RESD 1
    ._ebp:   RESD 1
    ._esp_lol:RESD 1
    ._size:
ENDSTRUC

global EnterVM86

[section .bss]
Buffer:
    RESB    TF._size

[section .text]

;Re-enter caller of EnterV86
ShootdownV86:

;16-bit pre-emptive VM -> INT 21H -> 32-bit kernel -> Serviced in VM86
;Stack frame is not created using ENTER because
;ESP+4 is the first argument
EnterV86:
    ;Save context

    ;Loading the registers
    mov    ebx,[esp+4]        ; Get pointer to register dump
    mov    eax,[ebx+4+_eax]

    mov    ecx,[ebx+TF._ecx]
    mov    edx,[ebx+TF._edx]
    mov    esi,[ebx+TF._esi]
    mov    edi,[ebx+TF._edi]
    mov    ebp,[ebx+TF._ebp]

    ;Push SS, ESP, EFLAGS, CS, EIP
    push   dword [ebx+TS._ss]
    push   dword [ebx+TS._esp]
    push   dword [ebx+TS_.eflags]
    push   dword [ebx+TS._cs]
    push   dword [ebx+TS._eip]
    mov    ebx,[ebx+TF._ebx]
    iret ; Enter V86

[section .data]

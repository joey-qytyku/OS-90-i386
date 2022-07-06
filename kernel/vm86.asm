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


global EnterV86
global ShootdownV86

[section .bss]
Buffer:
    RESB    TF._size

[section .text]

;----------------------------
;Re-enter caller of EnterV86
ShootdownV86:
    cli
    ;Load the context saved by EnterV86
    mov    ebx,Buffer        ; Get pointer to register dump
    mov    eax,[ebx+TF._eax]

    mov    ecx,[ebx+TF._ecx]
    mov    edx,[ebx+TF._edx]
    mov    esi,[ebx+TF._esi]
    mov    edi,[ebx+TF._edi]
    mov    ebp,[ebx+TF._ebp]
    mov    ebx,[ebx+TF._ebx]

    iret

;16-bit pre-emptive VM -> INT 21H -> 32-bit kernel -> Serviced in VM86
;Stack frame is not created using ENTER because
;ESP+4 is the first argument
EnterV86:
    ;Save context

    ;Load the registers
    mov    ebx,[esp+4]        ; Get pointer to register dump
    mov    [EnterContext.goto], dword .goto
    jmp    EnterContext
.goto
    iret ; Enter V86

EnterContext:
    mov    eax,[ebx+TF._eax]
    mov    ecx,[ebx+TF._ecx]
    mov    edx,[ebx+TF._edx]
    mov    esi,[ebx+TF._esi]
    mov    edi,[ebx+TF._edi]
    mov    ebp,[ebx+TF._ebp]

    ;Push SS, ESP, EFLAGS, CS, EIP
    push   dword [ebx+TF._ss]
    push   dword [ebx+TF._esp]
    push   dword [ebx+TF._eflags]
    push   dword [ebx+TF._cs]
    push   dword [ebx+TF._eip]
    mov    ebx,  [ebx+TF._ebx]
    jmp    near  [.goto]
.goto:
    DD     0

[section .data]

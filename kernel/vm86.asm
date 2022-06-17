STRUC TF
    _ss:    RESD 1
    _esp:   RESD 1
    _eflags:RESD 1
    _cs:    RESD 1
    _eip:   RESD 1
    _eax:   RESD 1
    _ebx:   RESD 1
    _ecx:   RESD 1
    _edx:   RESD 1
    _esi:   RESD 1
    _edi:   RESD 1
    _ebp:   RESD 1
    _esp:   RESD 1
ENDSTRUC

global EnterVM86
extern vm86_caused_gpf, emulate_svi, vm86_tss

[section .text]

;16-bit pre-emptive VM -> INT 21H -> 32-bit kernel -> Serviced in VM86

;Stack frame is not created using ENTER because
;ESP+4 is the first argument

EnterVM86:
    mov    ebx,[esp+4]
    mov    eax,[ebx+4+_eax]

    mov    ecx,[ebx+TF._ecx]
    mov    edx,[ebx+TF._edx]
    mov    esi,[ebx+TF._esi]
    mov    edi,[ebx+TF._edi]

    ;Push SS, ESP, EFLAGS, CS, EIP
    push   dword [ebx+TS._ss]
    push   dword [ebx+TS._esp]
    push   dword [ebx+TS_.eflags]
    push   dword [ebx+TS._cs]
    push   dword [ebx+TS._eip]
    mov    ebx,[ebx+TF._ebx]
    iret ; Enter V86

[section .data]

[section .bss]

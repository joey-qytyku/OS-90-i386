; Interrupts/Scheduler Communication
;

[section .data]

SingleTask:
    DB    0

SysClock:
.whole:
    resd    0
.fract:
    resd    0

;Update BIOS time in BDA?

LowHalfISR:
    push    ebp
    push    esp ; Nonsense
    push    edi
    push    esi
    push    edx
    push    ecx
    push    ebx
    push    eax

    ;In cdecl, arguments are cleared by the caller
    ;and locals are cleared by the callee
    lea     eax,[esp+48]
    push    eax
    call    MiddleDispatch
    add     esp,4 ; It's add not sub

    pop     eax
    pop     ebx
    pop     ecx
    pop     edx
    pop     esi
    pop     edi
    add     esp,4
    pop     ebp
    iret

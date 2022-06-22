; Interrupts/Scheduler Communication
;

[section .data]

TheIRQ:


[section .text]
;In case of a spurious interrupt, the ISR will be zero in the PIC
;chip that caused it, so the computer cannot differentiate between
;interrupts unless different ISRs are used


Low0:
    push    0
Low1:
    push    1

%assign i 3
%rep 16-3

Low%+i:
    push   i
    jmp    Continue
%endrep
Continue:
    pop     dword [TheIRQ]
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

    push    [TheIRQ]        ; To avoid the global variable :-)
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

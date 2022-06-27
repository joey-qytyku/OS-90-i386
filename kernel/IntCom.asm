; Interrupts/Scheduler Communication

[section .data]

[section .bss]

ErrorCode:
    DD  0

TheIRQ:
    RESD    1

[section .text]
EXTERN MiddleDispatch

;------------------------------------------------------------------
;In case of a spurious interrupt, the ISR will be zero in the PIC
;chip that caused it, so the computer cannot differentiate between
;interrupts unless different ISRs are used

Low0:
    push    dword 0
Low1:
    push    dword 1

%assign i 3
%rep 16-3
    EXTERN Low%+i
    Low%+i:
        push   i
        jmp    Continue
    %assign i i+1
%endrep

;-------------------
; Macros

%macro MakeExcept 1
    EXTERN %1
    Low%1:
%endm


;Requires the error code to be
;pushed off stack for exceptions that use it
%macro SaveTrapRegs
    push    ebp
    push    esp ; Nonsense
    push    edi
    push    esi
    push    edx
    push    ecx
    push    ebx
    push    eax


%endm

%macro RestTrapRegs
    pop     eax
    pop     ebx
    pop     ecx
    pop     edx
    pop     esi
    pop     edi
    add     esp,4
    pop     ebp 
%endm


;-----------------------
;Keep handling the IRQ
Continue:
    pop     dword [TheIRQ]
    SaveTrapRegs

    ;In cdecl, arguments are cleared by the caller
    ;and locals are cleared by the callee

    push    dword [TheIRQ]        ; To avoid the global variable :-)
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


LowDivide0:

LowDebug:

LowNMI:

LowBreakpoint:

LowOverflow:

LowBoundRangeExceeded:

LowInvalidOp:

LowDevNotAvail:

LowDoubleFault:

LowSegOverrun:

LowInvalidTSS:

LowSegNotPresent:

LowStackSegFault:

LowGeneralProtect:
    pop dword [ErrorCode]

LowPageFault:

LowFloatError:

LowAlignCheck:

;------------------------------------
; Other IA32 exceptions are not used


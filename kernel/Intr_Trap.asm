;-------------------
; Macros

%define NOCODE  0
%define ERRCODE 1

;Requires the error code to be
;pushed off stack for exceptions that use it
%macro SaveTrapRegs 0
    push    ebp
    push    esp ; Nonsense
    push    edi
    push    esi
    push    edx
    push    ecx
    push    ebx
    push    eax


%endm

%macro RestTrapRegs 0
    pop     eax
    pop     ebx
    pop     ecx
    pop     edx
    pop     esi
    pop     edi
    add     esp,4
    pop     ebp 
%endm

%macro MakeExcept 2
    EXTERN %1       ;Import the high handler
    GLOBAL Low%1    ;Export the low handler

    Low%1:
    %if %2 == ERRCODE
    pop     dword [ErrorCode]
    %endif

    SaveTrapRegs
    call    %1
    RestTrapRegs
    iret
%endm

[section .text]
EXTERN MiddleDispatch

;------------------------------------------------------------------
;In case of a spurious interrupt, the ISR will be zero in the PIC
;chip that caused it, so the computer cannot differentiate between
;interrupts unless different ISRs are used

%assign i 0
%rep 16
    Low %+ i:
        push   byte i ; 2 byte
        jmp    short Continue ; 2 byte
        ; -128-127 is reaches enough
    %assign i i+1
%endrep

EXTERN Low %+ i

;-----------------------
;Keep handling the IRQ
Continue:
    pop     dword [TheIRQ]
    SaveTrapRegs

    ;In cdecl, arguments are cleared by the caller
    ;and locals are cleared by the callee

    push    dword [TheIRQ]        ; To avoid the global variable
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

global GetErrCode
GetErrCode:
    mov     eax,[ErrorCode]
    ret


MakeExcept Divide0, NOCODE

MakeExcept Debug, NOCODE

MakeExcept NMI, NOCODE

MakeExcept Breakpoint, NOCODE

MakeExcept Overflow, NOCODE

MakeExcept BoundRangeExceeded, ERRCODE

MakeExcept InvalidOp, NOCODE

MakeExcept DevNotAvail, NOCODE

MakeExcept DoubleFault, ERRCODE

MakeExcept SegOverrun, NOCODE

MakeExcept InvalidTSS, ERRCODE

MakeExcept SegNotPresent, ERRCODE

MakeExcept StackSegFault, ERRCODE

MakeExcept GeneralProtect, ERRCODE
    pop dword [ErrorCode]

MakeExcept PageFault, ERRCODE

MakeExcept FloatError, NOCODE

LowAlignCheck:

;------------------------------------;
; Other IA32 exceptions are not used ;
;------------------------------------;

[section .data]

[section .bss]

;--------------------------------------------
;A table containing pointers to exceptions
;Not used for actual calling, addresses are
;copied to the IDT

ErrorCode:
    RESD    1

TheIRQ:
    RESD    1



%if 0
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with OS/90. If not, see <ttps://www.gnu.org/licenses/>.
%endif


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
EXTERN InMasterDispatch

;------------------------------------------------------------------
;In case of a spurious interrupt, the ISR will be zero in the PIC
;chip that caused it, so the computer cannot differentiate between
;interrupts unless different ISRs are used
;
;IRQ 2 and 9 are the same thing
;
%assign i 0
%rep 16
GLOBAL Low %+ i
    Low %+ i:
        push   byte i ; 2 byte
        jmp    short Continue ; 2 byte
        ; -128-127 is reaches enough
    %assign i i+1
%endrep

;-----------------------
;Keep handling the IRQ
Continue:
    pop     dword [TheIRQ]
    SaveTrapRegs

    ;reparam is used by InMasterDispatch
    lea     eax,[esp+48]
    mov     edx,[TheIRQ]
    call    InMasterDispatch
    add     esp,4

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

global _ErrorCode

_ErrorCode:
    RESD    1

TheIRQ:
    RESD    1



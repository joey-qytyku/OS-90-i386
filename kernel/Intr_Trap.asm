%if 0
     This file is part of OS/90.

    OS/90 is free software: you can redistribute it and/or modify it under the
    terms of the GNU General Public License as published by the Free Software
    Foundation, either version 2 of the License, or (at your option) any later
    version.

    OS/90 is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with OS/90. If not, see <https://www.gnu.org/licenses/>.
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

;The data segment registers of the V86 task are saved on the stack by the
;CPU, but the segment selectors are destroyed except for SS and CS,
;which are set to the TSS and interrupt gate values.
;Setting the segment selectors to proper values is simple. In the kernel,
;SS=DS, so we can set the data segment right away. We should not do this
;if the context is 32-bit protected mode because that would reload the
;segment descriptor caches, which would be slow
;
;The zero flag is set if the VM bit is off.
;
%define RestoreSegsIfNecessary
    mov     eax,[esp+32+12] ; Get EFLAGS????
    test    eax,1<<17
    jz      %%nvm

    ;The 32-bit operand size has no effect, but is less bytes
    mov     eax,ss
    mov     ds,eax
    mov     es,eax
    mov     fs,eax
    mov     gs,eax

%%nvm:
%endmacro

%macro MakeExcept 2
    EXTERN %1       ;Import the high handler
    GLOBAL Low%1    ;Export the low handler

    ;Write the error code to a variable if there is one for this exception
    Low%1:
    %if %2 == ERRCODE
    pop     dword [_ErrorCode]
    %endif

    SaveTrapRegs
    RestoreSegsIfNecessary

    ;Push pointer to the interrupt frame
    ;First of all, the stack if four bytes behind the
    ;

    ;Were we in V86 mode before the exception?
    ;The first C argument tells us if the exception did happen
    ;during V86 (aka set to 1).
    setz    al
    push    eax

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
;------------------------------------------------------------------
;Each handler is aligned to eight bytes.
;
;IRQ 2 and 9 are the same thing

%assign i 0
%rep 16
        align   8
GLOBAL Low %+ i
    Low %+ i:
        push   byte i ; 2 byte
        jmp    short Continue ; 2 byte
        ; -128 - 127 is reaches enough
    %assign i i+1
%endrep

    ;-----------------------
    ;Keep handling the IRQ
Continue:
    pop     dword [TheIRQ]
    SaveTrapRegs

    ;Did the interrupt stop a V86 task?
    ;If so, we do the same thing as with exceptions

    ;U sure?
    ;reparam is used by InMasterDispatch
    lea     eax,[esp+48]
    mov     edx,[TheIRQ]
    call    InMasterDispatch
    add     esp,4

    RestTrapRegs
    iret

global GetErrCode
GetErrCode:
    mov     eax,[_ErrorCode]
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
    pop dword [_ErrorCode]

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



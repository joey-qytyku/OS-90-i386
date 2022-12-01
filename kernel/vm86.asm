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
    ._size:
ENDSTRUC

global ScEnterV86
global ScShootdownV86

[section .text]

;-----------------------------
;Re-enter caller of ScEnterV86
ScShootdownV86:
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

;--------------------------------------------------------------
; Brief: Enter V86
;
; TODO: Serious refactoring underway


ScEnterV86:
        ; Create a "global variable" space in the stack, 52 bytes
        ; plus 4 because ESP points to next address to place new element
        ; This will store the register state before entering V86
        ; It is up to the caller if it wants to make a copy of this context
        ; or allow it to be clobbered

        lea     esp,[esp+56]        ; Does not change flags

        ;Save the current registers, all may be clobbered in V86
        ;ESP in the regular x86 register order is not present
        ;because a trap frame will have that come first, like as
        ;a result of an interrupt

        mov     [esp + TF._eax], eax
        mov     [esp + TF._ebx], ebx
        mov     [esp + TF._ecx], ecx
        mov     [esp + TF._edx], edx
        mov     [esp + TF._esi], esi
        mov     [esp + TF._edi], edi
        mov     [esp + TF._ebp], ebp
        pushf
        pop     [esp + TF._eflags]
        mov     [esp + TF._esp], esp    ;Do not remove
        ;The stack pointer must be saved

        ;The stack segment is treated differently in V86 and protected mode
        ;It must be set upon entry using IRET.
        ;When V86 causes a GPF, the state, including ESP, EFLAGS, EIP are saved
        ;on the stack and match with the location in the trap frame structure

        ;The stack is in the state that it must be after calling ShootdownV86
        ;Now it is time to enter V86 using the context passed to the function
        ;The argument has been pushed to the stack, the location was equal to
        ;ESP-4. Then I added 56 to the address. I have to go to ESP-(4+56), so
        ;ESP-60 is the address now.

        ;Now, we load the register parameters of the caller. As stated in the
        ;brief, saving registers from clobbering is the responsibility of the
        ;caller only. Anyway, ESP-60 is address of the pointer arg.

        mov     eax,[esp-60]
        mov     eax, [eax + TF._eax]
        mov     ebx, [eax + TF._ebx]
        mov     ecx, [eax + TF._ecx]
        mov     edx, [eax + TF._edx]
        mov     esi, [eax + TF._esi]
        mov     edi, [eax + TF._edi]
        mov     [eax + TF._ebp], ebp
        mov     [eax + TF._esp], esp


        ;Now it is time to enter V86. To get in, I will push EFLAGS,
        ;SS, ESP, and EIP and run IRET.

        ;The EFLAGS pushed must have the VM bit enabled
        ;but the rest of the state passed should be saved


[section .data]

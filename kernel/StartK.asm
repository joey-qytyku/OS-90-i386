extern LKR_STARTBSS
extern LKR_END
extern KernelMain
extern gdtr,idtr

;Problems: IO functions are broken or something?

section	.init
        ;If DOS does not find the MZ signature in an EXE file
        ;it may load it as a COM file, which are terminated with
        ;a return instruction, the loader jumps over this
        ret

Begin:
        ; Zero the BSS section
        mov     ecx,LKR_END
        sub     ecx,LKR_STARTBSS
        shr     ecx,2
        mov     edi,LKR_STARTBSS
        rep     stosd

        lgdt    [gdtr]
        lidt    [ldtr]

        mov     ax,3 <<3
        mov     ds,ax
        mov     es,ax
        mov     ss,ax
        xor     ax,ax
        mov     fs,ax   ;I will not use these for anything
        mov     gs,ax

        jmp    8h:Cont
Cont:
        mov     esp,InitStack  ; Set up a stack
        ;Far call not used because GCC cant return
        call    KernelMain

L:        hlt     ; Nothing to do now, halt
        jmp L   ; if interrupted, handle and halt again

section	.bss
        ;The initialization stack is used only for startup
        ;Processes get independent kernel stacks
        align   8
        resb    1024
InitStack:

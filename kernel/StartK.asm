extern LKR_STARTBSS
extern LKR_END
extern KernelMain
extern gdtr,idtr
extern test

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

        mov     al,[test]

        lgdt    [gdtr]
        jmp $
        lidt    [idtr]
        lldt    [null_ldtr]

        mov     ax,3<<3
        mov     ds,ax
        mov     es,ax
        mov     ss,ax

        ;I may not use these for anything
        xor     ax,ax
        mov     fs,ax
        mov     gs,ax

        jmp    8h:Cont
Cont:
        mov     esp,InitStack   ; Set up a stack
        call    KernelMain      ; GCC does not far return

L:      hlt     ; Nothing to do now, halt
        jmp L   ; if interrupted, handle and halt again

;---------------------------------
;The LDT is never used by the OS
;but it must be loaded anyway
;
null_ldt:
        DQ      0       ;Null descriptor
null_ldtr:
        DW      8
        DD      null_ldt


section	.bss
        ;The initialization stack is used only for startup
        ;Processes get independent kernel stacks
        align   8
        resb    1024
InitStack:

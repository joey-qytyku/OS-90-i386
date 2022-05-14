extern LKR_STARTBSS
extern LKR_END
extern KernelMain
extern IntDescTab

section	.init
        ;If DOS does not find the MZ signature in an EXE file
        ;it may load it as a COM file, which are terminated with
        ;a return instruction, the loader jumps over this
        ret

        ; Zero the BSS section
        mov     ecx,LKR_END
        sub     ecx,LKR_STARTBSS
        shr     ecx,2
        mov     edi,LKR_STARTBSS
        rep     lodsd

	mov	esp, InitStack  ; Set up a stack
	call	KernelMain

L:      hlt     ; Nothing to do now, halt
	jmp L   ; if interrupted, handle and halt again

section	.bss
        ; GCC requires aligned stack, usually 8
        align   8
        resb    4096
InitStack:

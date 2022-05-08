extern LKR_STARTBSS
extern LKR_END
extern KernelMain
extern IntDescTab
extern idtr
extern gdtr

section	.init
        ; Zero the BSS section
        xor     eax,eax
        mov     ecx,(LKR_END-LKR_STARTBSS)/4
        mov     edi,LKR_STARTBSS
        rep     lodsd

	mov	esp, InitStack	; Set up a stack
	call	KernelMain

L       hlt     ; Nothing to do now, halt
	jmp     ; if interrupted, handle and halt again

section	.bss
        ; GCC requires aligned stack, usually 8
        align   8
        resb    4096
InitStack:

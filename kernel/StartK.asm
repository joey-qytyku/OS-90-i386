; Kernel starts in real mode

extern __start_bss
extern KernelMain
extern IntDescTab
extern Idtr
extern Gdtr

section	.init
	mov	esp, InitStack	; Set up a stack
	call	KernelMain

	L hlt	; Nothing to do now, halt
	jmp L	; if interrupted, handle and halt again

section	.bss
	; GCC requires aligned stack, usually 8
	align 8
	resb 4096
	InitStack:

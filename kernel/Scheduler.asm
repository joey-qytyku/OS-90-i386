	TSS1	EQU

	extern	VM86TSS
	extern	GPF_NoVM86
	extern	CurrProcID
	;--------------------
	; Exception handlers
Divide0:

Overrun:
	; Segment overruns for FPU
	; For i386

Overflow:

NonMask:
	iret

DoubleFault:
	; Fatal error
	cli
	iret

GeneralProtect:
	;Check if it is a VM86 task
	iret

	;---------------
	; VM86 routines

EntrVM86:
	;Set TSS.ESP0
	mov	[Sv16_TSS+4],Sv16Stack
	;Set TSS.ESP
	cli

ExitVM86:

	section .bss

	resb	4096
Sv16Stack:


Sv16_TSS:
	resb	103

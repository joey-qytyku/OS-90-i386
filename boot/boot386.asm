; BOOT386, the OS/90 bootloader
; Assemble as BOOT386.SYS
; Should be the last driver loaded
; In config, the very last parameter

; See MS-DOS repo on github for info on drivers

;######## DEFS ########

RETDOS	EQU	'1'
BOOT	EQU	'2'

OPEN	EQU
SEEK	EQU
CLOSE	EQU
READ	EQU
PUTSTR	EQU	9

;#########################
;######Device header######
;#########################

; There is no next driver, also only used by DOS

NextDriver:
	DD	-1
	DW	8000h	;Device attribute word, chardev
	DW	Strategy	;Strategy entry point offset
	DW	Interrupt	;Interrupt entry point offset
	DB	"BOOT    "	;Driver name

Strategy:
	;Strat is not supposed to make INT 21H calls

	;Exit strat routine
	retf


Interrupt:
	; Load the kernel upon init interrupt
	; Further interrupts will not occur
	jmp $
;#############################
;############Data#############
;#############################

Message:	DB	"Starting OS/90",10,13,'$'
Path:	DB	"\OS90\KERNL386.EXE",0
CODE SEGMENT

PUBLIC global_dtab

begin:
	.86
	mov	boot_info_seg, es
	mov	boot_info_off, dx

	; Set GDT values of the APM segments


	; Load GDT
	lgdt [global_dtab]

	; Switch to protected mode
	mov	eax, cr0
	inc	al
	mov	cr0, eax

	.386
	; Set BSS to zero


CODE ENDS

DATA SEGMENT
	ALIGN 8
global_dtab:
; Null Segment
	dd 0,0
; 16-bit code
; 32-bit code fo

end_gdt:


DATA ENDS


BSS SEGMENT

boot_info_seg resw 1
boot_info_off resw 1

BSS ENDS

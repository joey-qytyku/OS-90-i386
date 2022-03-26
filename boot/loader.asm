;---------------------------------------------
; Loader for kuDOS, runs in MS-DOS
; Can be loadfixed. XMS should be disabled.
;---------------------------------------------
; The kernel is loaded "cold turkey", it must
; handle information gathering on its own
	; Loads higher half kernel at physical address 110000h
SEEK_SET	equ	0
SEEK_CUR	equ	1
SEEK_END	equ	2
LOAD	equ	110000h

	org	100h
main:
.check_8086:
	; Set invalid opcode exception, this exist in 80186+
	mov	ax,2506h
	mov	dx,error
	int	21h
	; 186 was never used for personal computers
	; If an 80186 instruction is supported, CPU >= 80286

	; Size prefix overrides do not work on the i286
	; If this fails, then this is a 286
	mov	eax,4300h

	; XMS equires at least a 286, so checking after CPU
	int	2Fh
	cmp	al,80h
	je	error

	call	enA20
	call	load_kernel
error:
	mov	ah,9
	mov	dx,.msg
	int	21h	; Print error message
	int	20h
.msg:	DB	"Error loading kuDOS",10,13,'$'

;----------------|Bootstrap routines|----------------

load_kernel:
	mov	ah,3Dh
	mov	al,1_000_0_000b
	mov	dx,kernel
	int	21h
	jc	error
	; BX now has file handle

	; Get size of the kernel image

	mov	ah,4202h
	xor	cx,cx
	mov	dx,cx
	int	21h
	jc	error

	; DX:AX contains new position

	; Save file size as DWORD?
	mov	[file_size],ax
	mov	[file_size+2],dx

	jmp $

	;------------------------------------------------
	; DX:AX contains the new file pointer
	; It also represents the size of the kernel image
	;------------------------------------------------
	; The algorithm:
	; If current seek equals file size, return
	; * Read a page
	; * Copy to extended memory using INT 15H
	; * Increase extended memory pointer by 4096
	;------------------------------------------------

.load_loop:
	; Read 4096 bytes from kernel image
	int	21h

	; Generate page directory

	; Load in CR3
	ret	; END OF LOAD KERNEL

enA20:	cli
	mov	si,64h
	mov	di,60h

	; Command: Disable keyboard
	mov	al,0ADh
	mov	dx,si ; 64h
	call	write_8042

	; Command: read output port register
	mov	al,0D0h
	mov	dx,si ; 64h
	call	write_8042

	; Now in 60h, set bit 1
	call	read_60h_once
	or	al,2
	push	ax

	; Command: write to output port
	mov	al,0D1h
	mov	dx,si
	call	write_8042
	pop	ax	; Get ouptput port byte back

	; Send it to 60h
	mov	dx,di
	call	write_8042

	; Command: re-enable the keyboard
	mov	al,0AEh
	mov	dx,64h
	call	write_8042
	sti
	ret

write_8042:
	; AL=Data, DX=Port
	push	ax
.l:
	in	al,64h
	shr	al,2
	jc	.l
	pop	ax
	out	64h,al
	ret

read_60h_once:
	in	al,64h
	shr	al,1
	jnc	read_60h_once
	ret

;-----------------Data-----------------

%assign i_pt0 0
	; IDK the physical addresses of the page tables
	; they are computed with the segment registers
init_pd:	ALIGN	4096
times 1024	DD 0

; This page table is attached to entry 768 of the page directory
init_pt0:	ALIGN	4096
%rep 1024
	DD	19 | (LOAD + (i_pt0<<12))
	%assign i_pt0 i_pt0+1
%endrep

init_pt1:

	ALIGN 4096
init_pt2:

;---------------------------
; For loading the kernel
buffer: 	times 4096 DB 0

kernel:	DB	"\nd\kernel.bin",0	; Kernel name

file_size:	DD	0

bios_gdt:
	; Null segment
	DQ	0
	; Data section with GDT, set to zero
	DQ	0
	; Source GDT
	DW	0FFFFh	; Limit
	DB	0,0,0	; 24-bit Address
	DB	93h	; Access rights
	DW	0	; Reserved in 286
	; Destination GDT
	DW	0FFFFh	; Limit
	DB	0,0,0	; 24-bit Address
	DB	93h	; Access rights
	DW	0	; Reserved in 286

	; BIOS code, stack, temp user code and BIOS code
	DQ	4

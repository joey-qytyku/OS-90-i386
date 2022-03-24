;---------------------------------------------
; Loader for kuDOS, runs in MS-DOS
; Can be loadfixed. XMS should be disabled.
;---------------------------------------------
; The kernel is loaded "cold turkey", it must
; handle information gathering on its own

	; Run this as a normal program
	; Loads higher half kernel at 110000h
SEEK_END	equ	2
loadat	equ	110000h

	org	100h
main:
.check_8086:
	cli
	mov	ax,2506h
	mov	dx,error
	int	21h
	sti
	; 186 was never used for personal computers
	; If an 80186 instruction is supported, CPU >= 80286

	; Size prefix overrides do not work on the i286
	; If this fails, then this is probably a 286
	mov	eax,eax

	; XMS equires at least a 286
	mov	ax,4300h
	int	2Fh
	cmp	al,80h
	je	error

	jmp	$

	call	enA20
	call	load_kernel
	jmp	(11000h>>4):0

error:
	mov	ah,9
	mov	dx,.msg
	int	21h	; Print error message
	int	20h
.msg:	DB	"Error loading kuDOS. Try: ",10,13
	DB	0F9h," Disable XMS",10,13
	DB	0F9h," Check if processor is an i386 or better",10,13,'$'

;########### Bootstrap routines ############

load_kernel:
	mov	ah,3Dh
	xor	al,al	; Read only
	mov	dx,kernel
	int	21h
	jc	error
	; BX now has file handle

	; Get file size
	mov	ah,42h
	xor	al,al ; SEEK_END
	xor	dx,dx
	xor	cx,cx
	jc	error

	; Set current seek location
	mov	ah,42h
	mov	al,SEEK_END
	xor	cx,cx
	xor	dx,dx
	int	21h
	jc	error
	mov	[file_size],dx
	mov	[file_size+2],cx
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

	ret

generate_ptab:
	; Generate page table

	; Load in CR3, will be used when switching to pmode
	ret

enA20	cli
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
	pop	ax	; Get ouptut port byte back

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

;########## DATA ##########


buffer:	times 4096 DB 0
kernel:	DW	"KERNEL.BIN"	; Kernel name

file_size:DD	0

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

	; Run this as a normal program
	; Do not load DOS high

loadat	equ	100000h
	org	100h
main:
	; Is XMS available?
	mov	ax,4300h
	int	2Fh
	cmp	al, 80h
	jne	mem_error

	call	enA20
	call	setup_apm
	call	setup_video
	call	extend_es
	call	load_kernel
	jmp	goto_kernel

file_error:
	mov	dx,ferr_msg
	jmp	exit
mem_error:
	mov	dx,merr_msg

exit:	mov	ah,9
	int	21h	; Print error message
	mov	ax,4C00h
	int	21h

;########### Bootstrap routines ############

setup_video:
	; Switch to 640x480 16 color
	mov	ax,12h
	int	10h

goto_kernel:
	; No return

load_kernel:
	; Open kernel FCB
	mov	ah,3Dh
	xor	al,al	; Read only
	mov	dx,kernel
	int	21h
	jc	file_error
	; BX now has file handle

	; Get file size
	mov	ah,42h
	xor	al,al ; SEEK_END
	xor	dx,dx
	xor	cx,cx
	jc	file_error
	; DX:AX now contains the offset realtive to BOF
	; AKA the file size in this case
	mov	cx,8000h
	; DIV.W is actually 32-bit, it uses DX:AX
	; Furthermore, divisor is 16-bit
	div	cx
	; Result is the total number of 4K blocks
	; Now in AX

	; BX still has the handle

	mov	si,ax
	xor	di,di
	mov	dx,buffer
; I do not

.l:
	; Add 4096 to file pointer (in DX:CX)
	mov	ah,3Fh
	add	cx,4096
	adc	dx,0
	int	21h

	; Read into the buffer

	; Copy the buffer

	inc	si
	cmp	si,di
	jz	.end
	jmp	.l

.end:	ret

extend_es:
	; Unreal mode setup
	xor	ebx,ebx	; Create linear address
	mov	bx,ds
	shl	ebx,4
	add	ebx,gdt_begin
	mov	[gdtr_val+2],ebx

	cli
	push	es
	lgdt	[gdtr_val]
	mov	eax,cr0
	inc	ax

	mov	cr0,eax
	jmp	$+2

	mov	ax,8h
	mov	es,ax

	mov	eax,cr0
	dec	ax
	mov	cr0,eax

	pop	es
	sti
	; The extension is permanent until caches are changed again
	ret

apm_setup:
	int	15h
	ret
enA20:

	cli
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

;######### DATA ###########

ferr_msg:	DB	"File error",10,13,36
merr_msg:	DB	"Memory error",10,13,36
kernel:	DB	"KERNEL.BIN",0

gdt_begin:
	DD	0,0
	DB	0xff,0xff,0,0,0,10010010b,11001111b,0
gdt_end:
gdtr_val:	DW	gdt_end-gdt_begin-1
	DD	0

kernel_pages:
	DW	0

handl:	DW	0

buffer:	times 4096 DB 0

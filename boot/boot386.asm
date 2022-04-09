	FORMAT  binary
	ORG	100h

Begin:
	mov	ax,cs
	jmp $

	mov	ah,3Dh
	mov	al,0
	mov	dx,Name
	int	21h

	jc	 FileError

	mov	bx,ax
	mov	ax,4202h
	xor	cx,cx
	mov	dx,cx


Name:   DB	"KERNL386.SYS",0

FileError:

GdtInfo:	DW	 64
	DD	 100000h

GdtCopy:

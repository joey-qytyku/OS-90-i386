	org	100h

	mov	ah,9
	mov	dx,512
	int	2Fh

	cmp	al,1
	jnz	error



	ret

error:
	ret

handle:
	DW	0


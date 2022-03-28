; Load kernel.bin in low memory
; At least 384K of memory must be free, or 6 segments
; All conventional memory is reserved when this happens
; Because that is what COM programs do when they run

	org	256
	mem	EQU 60000h

main:
	;------------------------------------------------
	; DOS memory allocations are not page aligned
	; This means that the resulting allocation block
	; address will likely need to be aligned
	; This is done with <<8, that means at most
	; 256 bytes may be unused
	; So the required memory is actuallly mem+256
	;------------------------------------------------





;---------------
; Data Section

ErrMSG:	DB	"Error loading kernel in low memory"

path:	DB	"\nd\kernel.bin"

init_pt0:	ALIGN	4096
%rep 1024
	DD	19 | (LOAD + (i_pt0<<12))
	%assign i_pt0 i_pt0+1
%endrep

init_pt1:
	ALIGN 4096
init_pt2:


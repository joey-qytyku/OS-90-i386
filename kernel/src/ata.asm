include 'include/ata.inc'



section '.data'

	; Use bit test
virtual_bus_sel:
	DB	0

dsel_ata0	DB	0
dsel_ata1	DB	0

section '.text'

DriveSelect:
	; DX=Control port of ATA drive
	; If the drive select is the same as current
	ret

ATA28_read:
	; EDX=Port
	; AX=Sector count c
	; EDI=Buffer

	mov	ecx, 256
	rep	insw
	ret

macro InitPIO
{
}

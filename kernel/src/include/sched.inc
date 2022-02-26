include "kerndefs.h"

struct TSS_Struct {
	_link
	_ss0
	_esp2
	_ss1
	_esp1
	_ss2
	_esp2
	_eflags
	_eip
	_cr3
	_edi
	_esi
	_ebp
	_esp
	_ebx
	_edx
	_ecx
	_eax
	_gs
	_fs
	_ds
	_ss
	_cs
	_es
}

CF=0x1
RES=0x2
PF=0x4
AF=0x10
ZF=0x40
SF=0x80
TF=0x4
IF=0x8
DF=0x10
OF=0x40
IOPL=0x80

MODE_KERNEL=0
MODE_INTERRUPT=1
MODE_USER=2




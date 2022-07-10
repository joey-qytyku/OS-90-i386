GDTDEF_R0_CSEG = 0x00cf9a000000ffffULL
GDTDEF_R0_DSEG = 0x00cf92000000ffffULL
GDTDEF_R3_CSEG = 0x00cff8000000ffffULL
GDTDEF_R3_DSEG = 0x000ff2000000ffffULL
GDTDEF_R0_TSS  = 0x0000890000002068ULL


Prefixes can be combined. For example:

DI_ProbleLines
DS_Vint86
DF_OpenFile


A - IBM PC/IA32 or configuration
D - Function available to driver
E - Link/load
F - Filesystem
I - Interrupts
L - Mutex Lock/Unlock or critical section
M - Memory manager
S - Scheduler

SECTION '.bss'

TaskStateSegment:
        rb 104 + 8192


SECTION '.data'

qGlobalDescriptorTable:
        DQ      GDTDEF_R0_CSEG
        DQ      GDTDEF_R0_DSEG
        DQ      GDTDEF_R3_CSEG
        DQ      GDTDEF_R3_DSEG
        DQ      GDTDEF_R0_TSS
.end:

PointerToGTD:
        DW      qGlobalDescriptorTable.end - qGlobalDescriptorTable - 1
        DD      qGlobalDescriptorTable

AL_AcquireLDT:

        ; IN: EAX=Address EBX=Descriptor ptr
A_InsertAddressToDesc:
        mov     [ebx+2],ax
        shr     eax,8
        mov     [ebx+4],al
        mov     al,ah
        mov     [ebx+7],al
        ret

MkTaskSeg:
        mov     eax,qGlobalDescriptorTable+(4*4)
        call    A_InsertAddressToDesc

        ; IN:
MkIntrGate:

section '.init'

StartK:
        lgdt    [PointerToGDT]

        ret

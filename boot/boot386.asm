%if 0

BOOT386, the OS/90 bootloader written by Joey Qytyku

There is no license, feel free to
use and modify and use it to load other kernels.

- HISTORY -

22 April 2022:
        Added in and fixed Enable-A20 code
23 April 2022:
        Converted to NASM code
20 April 2022:
        Switched to XMS and .COM program
        Removed enable A20 code, XMS used instead
        Updated file macros

1 May 2022:
        Updated enter protected mode routine
        Fully removed A20 code
7 May 2022:
        Fixed A20 handling code
        Added page table copies
        HMA cannot longer be shared
        Removed old error messages
11 May 2022
        Fixed many bugs
        Changed debugging macros
        Added in some file load code
13/14 May 2022
        Added loader code
        Fixed bug with XMS calls
        It appears to be loading into memory
%endif

;-----------------------------
; Equates

%define X2LN 0CDh
%define LNE 10,13,'$'

PAGE_SHIFT      EQU	12
PUTSTR          EQU	byte 9
EXIT            EQU	byte 4Ch
OPEN_RO         EQU	byte 3D00h
CLOSE           EQU	byte 3Eh
READ            EQU	byte 3Fh

SEEK_SET	EQU	word 4200h
SEEK_CUR	EQU	word 4201h
SEEK_END	EQU	word 4202h

        ORG	100h
        jmp	Main

Weclome:        DB      "Starting OS/90",10,13
times 14        DB      X2LN
DB      LNE

;----------------------------
; Error message strings

MnoXMS          DB	"[!] OS/90 required an XMS driver.",LNE
OpenErr         DB      "[!] Error opening KERNL386.SYS, reinstall OS/90.",LNE
A20Error	DB      "[!] Error enabling A20 gate.",LNE
HMA_Error       DB	"[!] Could not get entire HMA.",LNE
ExtMemErr       DB      "[!] Could not allocate extended memory",LNE
Machine         DB      "[!] 2MB of memory is required",LNE
KernelFile      DB      "[!] The kernel image appears corrupted.",LNE
FileIO_Error    DB      "[!] Error opening or reading from kernel image",LNE
MoveError       DB      "[!] Error copying from conventional to extended memory",LNE


;----------------------------
; Debug message strings

%macro ERROR 1
        mov     ah,9
        mov	dx,%1
        int	21h
        mov	ax,4CFFh
        int	21h
%endmacro

;Greedy macro parameters
%macro MESSAGE 1+
        pusha
        jmp     %%c

%%m:    DB      %1
%%c:    mov     ah,9
        mov     dx,%%m
        int     21h
        popa
%endmacro

%macro xms 1
        call far %1
%endmacro

Main:
        cld
        ;Clear screen with mode switch
        mov     ax,3
        int     10h

        ;Print welcome message
        mov	ah,9
        mov	dx,Weclome
        int	21h

        ;XMS present
        mov	ax,4300h
        int	2Fh
        cmp	al,80h
        je	Present

        ; NO XMS PRESENT: ERROR
        ERROR   MnoXMS
Present:
        ;Acquire XMS far pointer
        push    es
        mov	ax,4310h
        int	2Fh

        mov	bp,XMS
        mov	[bp],bx
        mov	[bp+2],es
        pop     es

        ;Query A20, is it already enabled
        mov     ah,7
        xms     [bp]
        cmp     al,0
        jz      EnableA20       ; Not already enabled
        jmp     A20AlreadyOn

EnableA20:
        ;Global enable A20 gate
        mov     ah,3
        xms     [bp]
        cmp     al,1
        je      A20Enabled

        ERROR   A20Error

A20AlreadyOn:
        MESSAGE "[i] A20 is already enabled",LNE
A20Enabled:
        MESSAGE "[i] Siezing the high memory area",LNE
        mov     ah,1
        mov     dx,0FFFFh
        xms     [bp]
        cmp     al,1
        je      HMA_OK
        ERROR   HMA_Error
HMA_OK:

PageSetup:
        ;Note: Should I put it here or delete
        MESSAGE "[i] Setting up paging (stage 1)",LNE
        push    es
        mov     ax,0FFFFh
        mov     es,ax

        mov     bx,16

        ;Zero all four page tables/dir
        ;So that #PF can happen instead of
        ;something much worse
        mov     cx,16384/4
        xor     si,si
        xor     eax,eax
        rep     stosd

        ;Create the page directory
        mov     dword [es:bx],       (101h<<PAGE_SHIFT)|3
        mov     dword [es:bx+768*4], (102h<<PAGE_SHIFT)|13h
        ;Kernel has cache enabled
        ;Low 1M does not because VRAM should not be cached

        ;Copy the IDMAP page table to HMA
        mov     cx,4096/4
        mov     si,IDMap
        mov     di,4096
        rep     movsd

        ;CR3 lower bits are reserved, best to not touch them
        mov     eax,cr3
        and     eax,~(0FFFh)
        or      eax,100000h
        mov     cr3,eax
        xor     eax,eax

        ;How much extended memory - HMA
        ;Other functions will be used to get
        ;a more precise reading of extended memory
        ;for the purposes of the kernel
        mov     ah,8
        xms     [bp]

        ;Allocate all available extended memory
        mov     dx,ax
        mov     ah,9
        xms     [bp]
        cmp     al,1
        je      ExtAllocSuccess

        ERROR   ExtMemErr

ExtAllocSuccess:
        ;This will allocate at most 64M because of the 16-bit size
        ;there are incontiguities in the extended memory which can
        ;limit the size of the EMB. Regardless, the memory
        ;allocated by XMS driver will be more than enough for the
        ;kernel to be loaded into.

        ;Save EMB handle to extended move struct
        mov     [XMM.deshan],dx

        ;Lock the EMB, this ensures it does not move
        mov     ah,0Ch
        xms     [bp]

        ;Address of the EMB in DX:BX
        mov     ax,bx
        mov     cx,dx

        ; Store it, I will use it later
        mov     [KernelAddr],bx
        mov     [KernelAddr+2],dx

        ;Align CX:AX to page boundary
        add     ax,4095
        adc     cx,0
        and     ax,~4095

        ;Aligned minus Original is the offset
        ;in order to load at a page boundary
        sub     cx,dx
        sbb     ax,bx

        ;Check!!
        ;CX:AX is the offset to move to? EMB
        ;Copy it to memory move structure
        mov     [XMM.desoff],ax
        mov     [XMM.desoff+2],cx

LoadKernel:
        ;Open kernel file
        mov     ax,OPEN_RO
        mov     dx,Path
        int     21h
        jnc     .OpenGood
        ERROR   FileIO_Error
.OpenGood:
        ;Handle is in AX, it will not be clobbered in BX
        mov     bx,ax

        ;Seek to end to get size
        mov     ax,SEEK_END
        xor     cx,cx
        mov     dx,cx
        int     21h
        jnc     .FileOpened

.FileOpened:
        ;File byte size in DX:AX
        ;The kernel image is page granular
        ;aka it is in 4096 byte blocks
        shrd    ax,dx,PAGE_SHIFT
        ;AX now contains the page count, there is no
        ;way it does not fit in a 16-bit register (256M)
        test    ax,ax   ;Kernel should not be zero pages :)
        je      Corrupted
        mov     di,ax   ;DI will be the loop counter

        ;Seek back
        mov     ax,SEEK_SET
        xor     cx,cx ; Same as moving from a zero reg
        mov     dx,cx
        int     21h

        ;Set conventional memory pointer
        mov     word[XMM.srcoff],Buffer
        mov     [XMM.srcoff+2],ds

        ;BX remains the file pointer
        ;DI is the loop counter
        mov     si,XMM

        ;I am out of registers, so I will move 4096
        ;manually wherever needed
.loadloop:
        ;Read 4096 bytes into buffer
        mov     ah,READ
        mov     cx,4096
        mov     dx,Buffer
        int     21h

        ;Copy to extended memory
        ;HIMEM seems to zero BL on success
        push    bx
        mov     ah,0Bh
        xms     [XMS]
        cmp     al,1
        je      .copy_success
        ERROR   MoveError

.copy_success:
        pop     bx

        ;Seek 4096 bytes forward
        mov     ax,SEEK_CUR
        mov     dx,4096
        xor     cx,cx
        int     21h

        ;Add 4096 to the extended move offset
        add     [XMM.desoff],di

        dec     di
        jnz    .loadloop

        ;Close the file
        mov     ah,CLOSE
        int     21h

        jmp $

Page2:
        ;Map the higher half
        ;

GotoKernel:
        cli
        ; Get linear address of GDTR
        mov     ebx,ds
        shl     ebx,4
        add     ebx,_GDTR
        lgdt    [ebx]

        ;Pass the information block as a segment
        ;All real mode segments are 16-byte aligned
        ;so passing the segment works
        mov     dx,InfoStructure
        shr     dx,4
        mov     cx,ds
        add     dx,cx

        ;Switch to protected mode
        mov	eax,cr0
        or	eax,8000_0001h
        mov	cr0,eax
        jmp     $+2     ; Clear prefetch cache

        ;Bravo six, going dark
        use32
        mov     ax,10h
        mov     ds,ax
        mov     es,ax
        mov     ss,ax
        jmp	8h:0C000_0001h
        ;Jumps over the protective RET
        ;See StartK.asm for more information
        use16

Corrupted:
        ERROR   KernelFile

InfoGet:
        ;Is there a PCI bus?


        ;Extended memory
        ret

;#############################
;############Data#############
;#############################

Path:   DB      "\OS90\KERNL386.EXE",0

KernelAddr:
        DD      0

_GDTR:
        DW      _GDT.end - _GDT
        DD      0       ;Will figure out

_GDT:
        DQ	0
        ; Flat code segment
        DB      0FFh,0FFh,0,0,0,1_00_11010b,11_001111b,0
        ; Flat data segment
        DB      0FFh,0FFh,0,0,0,1_00_10010b,11_001111b,0
.end:

XMS:    DD      0
Handle: DW      0

XMM:    ;Extended memory move
.len:   DD      4096
.srchan:DW      0       ;Must be zero so that srcoff is a seg:off pair
.srcoff:DD      0       ;Figured out later
.deshan:DW      0
.desoff:DD      0

InfoStructure:
        DW      0       ; Size

;Page tables are zeroed before this is copied in
IDMap:
%assign i 0
%rep 256
        DD      (i << PAGE_SHIFT) | 13h
        %assign i i+1
%endrep

; COM programs get 64K, so this should be safe
Buffer:

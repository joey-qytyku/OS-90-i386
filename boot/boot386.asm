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
%endif

;-----------------------------
; Equates

PAGE_SHIFT      EQU	12
PUTSTR          EQU	9
EXIT            EQU	4Ch
OPEN_RO         EQU	3D00h
CLOSE           EQU	3Eh
READ            EQU	3Fh

SEEK_SET	EQU	4200h
SEEK_CUR	EQU	4201h
SEEK_END	EQU	4202h

XMS_SUCC        EQU     1

        ORG	100h
        jmp	Main
%define X2LN 0CDh
%define LNE 10,13,'$'

Weclome:        DB      "Starting OS/90",10,13
times 14        DB      X2LN
DB      LNE

;----------------------------
; Error message strings

MnoXMS          DB	"[!] XMS is required",LNE
OpenErr         DB      "[!] Error opening KERNL386.SYS",LNE
A20Error	DB      "[!] Error enabling A20 gate",LNE
HMA_Error	DB	"[!] Could not get entire HMA",LNE
ExtMemErr       DB      "[!] Could not allocate extended memory",LNE

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
        MESSAGE "[i] Setting up paging",LNE
        push    es
        mov     ax,0FFFFh
        mov     es,ax

        mov     bx,16

        ;Zero all four page tables/dir
        ;So that #PF happens
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

        ;Get total extended memory (excluding HMA)
        mov     ah,8
        xms     [bp]

        ;AX=Largest EMB size
        ;DX=Entire extended memory minus HMA

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

        ;Save EMB handle
        mov     [Handle],dx

        ;#### Idea correct, implementation may be wrong

        ;Lock the EMB
        mov     ah,0Ch
        xms     [bp]
        jmp $

        ;Address of the EMB in DX:BX
        mov     ax,bx
        mov     cx,dx

        ;Align CX:AX to page boundary
        add     ax,4095
        adc     cx,0
        and     cx,~4095

        ;Aligned minus Original is the offset
        sub     cx,dx
        sbb     ax,bx
        jmp $

GotoKernel:
        ; Get linear address of GDT
        mov     ebx,ds
        shl     ebx,4
        add     ebx,_GDTR
        lgdt    [ebx]

        ;Switch to 32-bit protected mode
        ;Cannot think of a better way to do this
        cli
        mov	eax,cr0
        or	eax,8000_0001h
        mov	cr0,eax

        ;*Hacker voice* I'm in
        use32
        mov     ax,10h
        mov     ds,ax
        mov     es,ax
        mov     ss,ax
        jmp	8h:0C000_0000h
        use16

;#############################
;############Data#############
;#############################

Path:   DB      "\OS90\KERNEL.386",0

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


;Copied to HMA
IDMap:
%assign i 0
%rep 256
        DD      (i << PAGE_SHIFT) | 13h
        %assign i i+1
%endrep
times 768       DD      0

;For map the kernel to 0xC0000000
;Or 1024 virtual pages at 0xC0000 to physical pages at 0x100000 
VMMap:

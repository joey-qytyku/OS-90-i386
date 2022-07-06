;IRQ grabber for OS/90
;Records the modification of interrupt vectors.
;Must be loaded before anything else
;Small conventional memory footprint

;DOS has a change vector function and also allows vectors to be modified by software.
;If a driver sets a vector to implement an IRQ, the kernel must detect it and call
;the 16-bit driver when the IRQ is recieved.

GINT EQU 35h
SINT EQU 25h
BASE_MASTER EQU 8h
BASE_SLAVE  EQU 70h

Header:
    DW  0
    DW  0
    DW  8000h
    DW  Strategy
    DW  Interrupt
    DB  "@#O9!_~2" ;Nonsense name so it cannot be called

Rest21h: DD 0
Error:   DB "Error caused by GRABIRQ",10,13,'$'

ModIRQ:
    DB  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

Strategy:
    retf

NewHandler:
    cmp ah,SINT
    jne .cont

    ;AL contains the vector being changed
    push ax
    pop ax

.cont:
    jmp [Rest21h]

Interrupt:
    ;This is used for the INIT call and nothing else
    cmp al,0
    jnz

    ;Store the INT 21H handler
    mov ah,GINT
    mov al,21h
    int 21h

    mov [Rest21h],bx
    mov [Rest21h+2],es

    ;Hook the 21H handler
    mov ah,SINT
    mov al,21h
    mov bx,NewHandler
    int 21h

    retf

NotINIT:
    mov dx,Error
    jmp $


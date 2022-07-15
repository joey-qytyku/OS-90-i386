
main:
        cli
        mov     ax,cs
        mov     ds,ax
        mov     ss,ax
        mov     sp,0FFF8h
        mov     ax,900h
        mov     es,ax
        sti

        push    6
        push    5
        push    2
        call    dword pnp

pnp:
    push ebp
    mov  ebp,esp
    std
    mov     ecx,[ebp+8]  ;Number of args
    lea     esi,[ebp+12] ;Start address, SECOND ARG

.L:
    jecxz   .Done

    ;Load the argument, ESI+4
    lodsd
    add     esi,6
    mov     word [es:esi],ax
    dec ecx

    jmp     .L


.Done:
jmp $

times 510-($-$$) DB 0
DW 0AA55h

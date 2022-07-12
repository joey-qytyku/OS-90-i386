
main:
        cli
        mov     ax,cs
        mov     ds,ax
        mov     ss,ax
        mov     sp,0FFF8h
        sti

        movd    mm0,[value]
        movd    mm1,[value2]
        paddb   mm1,mm0
        movd    eax,mm0
        emms
        jmp $


align 32
value: DQ       ~0
value2: DB 0,0,0,0,0,0,0,1

times 510-($-$$) DB 0
DW 0AA55h
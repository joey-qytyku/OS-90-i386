global PnInsertEntryPoint
global PnCallBiosInternal

;Variadic function are called by simply pushing
;the desired number of arguments on the (always) stack
;
; BIOS plug-and-play BIOS calls use 16-bit far calls
; and are required to handle both 32-bit and 16-bit
; stack segments.
; A separate data segment is required because PnP BIOS
; uses 16-bit offsets. (The dseg is passed on the stack
; for function that need it)

PnInsertEntryPoint:


;Uses regparam(1), no base pointer
;CArgs(dword argc, dword func, ...)
PnCallBiosInternal:
    ;In memory, arguments are in sequential order
    ;The following will copy the low word of each
    ;32-bit argument AFTER the first one back by one word
    ;------***-------------
    ; |A32|A32|A32|EIP|
    ; |---|-|A|A|A|EIP|
    ;----------------------
    ;Because stack addressing is relative, extra
    ;bytes will not cause any problems

    mov     ecx,eax
    lea     esi,[esp+4] ;First argument address
    std
.L:
    add     esi,4       ;Goes to second arg on 1st iteration
    mov     eax,[esi]
    mov     [esi+2],ax
    loop    .L

    ;Call PnP entry point

    ret

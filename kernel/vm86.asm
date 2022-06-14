global EnterVM86
extern vm86_caused_gpf, emulate_svi, vm86_tss

STRUC   Reg16
    _eax
    _ebx
    _ecx
    _edx
    _esp
    _ebp
    _ss
    _cs
    _eip
    _eflags
ENDSTRUC

[section .text]

;TODO: Integrate with scheduler?

;The DOS kernel cannot multitask or be pre-empted. When a API call
;takes place (or any 16-bit software interrupt) the call must be
;serviced with the scheduler temporarily in single task mode.
;this allows timer interrupts to continue running but the kernel will
;not switch tasks upon IRQ 0

;16-bit pre-emptive VM -> INT 21H -> 32-bit kernel -> Serviced in VM86


EnterVM86:
    ;Argument is a pointer to the register list
    ;Current flags preserved

    ;The VM86 mode runs, SS,CS,ESP,EIP must be set using IRET

[section .data]

[section .bss]

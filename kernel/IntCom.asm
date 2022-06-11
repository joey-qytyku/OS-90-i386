; Interrupts/Scheduler Communication
;

[section .data]

SysClock:
.whole:
    resd    0
.fract:
    resd    0

;Timer fires every milisecond
;the uptime is stored in fixed point format
;and is represents the hundreths
;this is a fudge factor because the timer
;interrupt is inaccurate for counting individual
;miliseconds

TimerISR:
    add    dword [SysClock+4],80000000h
    adc    dword [SysClock],0
    iret
;Update BIOS time?


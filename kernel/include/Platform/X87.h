// Monitor Co-processor
// If MP and TS are set, the WAIT instruction causes #NM
// (coprocessor/device not available). (also ESC).
// In other words, it decides if FWAIT is affected by the TS bit
#define CR0_MP (1<<1)

// Emulate
// If set, X87 instructions cause #UD so that they may be emulated
// Some kernels use it instead of lazy switching, so that
// FPU registers are saved and restored for a process
// that uses them, OS/90 uses lazy switching instead
#define CR0_EM (1<<2)

// Task switched, not used for software multitasking
// but affects FPU instructions, which cause a #UD so that
// the OS can save them when another task uses the FPU
// The scheduler sets TS to 1 on a context switch 
#define CR0_TS (1<<3)

// Extension type, 80286=0, >=80386=1
// If there is no FPU (EM=1), this bit means nothing
#define CR0_ET (1<<4)

// Native Exception
// If set, exceptions are reported using vector 7
// instead of IRQ#13. See more in Resource.c
#define CR0_NE (1<<5)

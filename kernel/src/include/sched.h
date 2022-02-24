#include "kerndefs.h"

typedef struct {
	long link;
	long ss0, esp0;
	long ss1, esp1;
	long ss2, esp2;
	long eflags, eip, cr3;
	long edi, esi, ebp, esp, ebx, edx, ecx, eax;
	long gs, fs, ds, ss, cs, es;
}tss_t;

// ESP0, assuming it is already aligned by 4
// will remain aligned by 4

typedef struct {
	unsigned long
		ss, // Only on stack switch
		esp,
		eflags,
		cs,
		eip,
		error_code;
}iframe;

typedef enum {
	M_KERNEL,
	M_INTERRUPT,
	M_USER
}mode;

inline iframe *get_iframe(void);
#pragma aux get_frame =\
	"lea esi, [esp-4"\
	modify [ esp ]\
	value [ esi ]

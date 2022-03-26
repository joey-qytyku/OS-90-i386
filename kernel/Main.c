#include <Type.h>

#define VM_32  0  // 32-bit process
#define VM_16U 1 // User/Concurrent VM
#define VM_16S 2 // Supervisor/Atomic VM

typedef struct __attribute__((packed))
{
	int i,j;
}IntD;

struct xDtr {
	dword address;
	word limit;
}__attribute__((packed));

typedef struct __attribute__((aligned (4)))
{
	long eax,ebx,ecx,edx,esi,edi,ebp,esp;
	dword eip,eflags;
	 VMType;
	unsigned char VMPages;
	short TimeSliceCounterMS;
	void *x87Environment;
}ProcCtlBlk; // Alignment ensures fast access

struct xDtr Gdtr, Idtr;


byte VM86TSS[103], MainTSS[103];
IntD IntDescTab[256];

static inline void ClearInterrupts(void) { __asm__ volatile ("cli"); }
static inline void SetInterrupts  (void) { __asm__ volatile ("sti"); }

static inline void OutpB(short port, char val)
{
	__asm__ volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

static inline void InpB(short port)
{
	byte ret;
	__asm__ volatile ( "inb %1, %0"
		:"=a"(ret)
		: "Nd"(port) );
	return ret;
}

void KernelMain(void)
{
	byte *msg = "Hello, VGA world";
	for (int i=0;msg[i]!=0;*(char*)(0xB8000+i)=msg[i],i+=2);
}

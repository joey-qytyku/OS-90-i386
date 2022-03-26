#include <Type.h>
#include <PIC.h>

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
	long eax,ebx,ecx,edx,esi,edi,ebp,esp3,esp0;
	dword eip,eflags;
	byte VMType;
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
	__asm__ volatile ( "outb %0, %1" :: "a"(val), "Nd"(port));
}

static inline byte InpB(short port)
{
	byte ret;
	__asm__ volatile ( "inb %1, %0" :"=a"(ret) : "Nd"(port));
	return ret;
}

void EarlyInitPIC(byte map_to)
{
	byte icw1 = ICW1 | CASCADE | ICW1_ICW4 | LEVEL_TRIGGER;
	// Note: Different bits tell OCWs and ICWs appart in CMD port

	// ICW1 to both PIC's
	OutpB(0x20, icw1);
	OutpB(0xA0, icw1);

	// ICW2, set interrupt vectors
	OutpB(0x21, map_to << 3);
	OutpB(0xA1, (map_to+8) << 3);

	OutpB(0x21, 4);	// ICW3, IRQ_2 is cascade
	OutpB(0xA1, 2);	// ICW3 is different for slave PIC, cascade IRQ#

	OutpB(0x21, ICW4_X86);
	OutpB(0xA1, ICW4_X86 | ICW4_SLAVE); // Assert PIC2 is slave
}

void KernelMain(void)
{
	sbyte msg[] = "Hello, VGA world";

	EarlyInitPIC(32);

	for (int i=0;msg[i]!=0;*(char*)(0xB8000+i)=msg[i],i+=2);
}

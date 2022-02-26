#include <kerndefs.h>

static volatile enum
	last_mode = MODE_KERNEL,
	curr_mode = MODE_KERNEL;

void set_mode(enum mode m) {
	last_mode = curr_mode;
	curr_mode = m;
}

void current_mode();

void icontx();

void count_dow();



_interrupt void i8254(void)
{
	outp(32, 32);
}

SHELL=/bin/bash

INCLUDE=~/gcc-11.1.0-nolibc/i386-linux/lib/gcc/i386-linux/11.1.0/include
CFLAGS=-I $(INCLUDE) -Os -nostartfiles -nodefaultlibs -ffreestanding -std=gnu99

TCHAIN=~/gcc-11.1.0-nolibc/i386-linux/bin

CC=$(TCHAIN)/i386-linux-gcc
LINK=$(TCHAIN)/i386-linux-ld

CSOURCE=$(wildcard *.c)
ASOURCE=$(wildcard *.asm)

AS=nasm
ASFLAGS=-felf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

kernel.bin:$(wildcard *.c)
	$(LINK) -T kernel.ld $(wildcard build/*.o) -o build/kernel.elf

all: kernel.bin

clean:
	rm *.o

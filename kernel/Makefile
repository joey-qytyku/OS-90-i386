SHELL=/bin/bash

INCLUDE=~/gcc-11.1.0-nolibc/i386-linux/lib/gcc/i386-linux/11.1.0/include
CFLAGS=-I $(INCLUDE) -Os -nostartfiles -nodefaultlibs -ffreestanding -std=gnu99 -Wpedantic

TCHAIN=~/gcc-11.1.0-nolibc/i386-linux/bin

CC=$(TCHAIN)/i386-linux-gcc
LINK=$(TCHAIN)/i386-linux-ld

CSOURCE=$(wildcard *.c)
ASOURCE=$(wildcard *.asm)

OBJ=$(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.asm,%.o,$(wildcard *.asm))

AS=nasm
ASFLAGS=-felf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o build/$@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o build/$@


Kernel.bin: $(OBJ)
	$(LINK) -T Kernel.ld $(addprefix build/, $<) -o build/Kernel.bin
	objcopy -S -O binary build/Kernel.bin build/Kernel.bin

all: Kernel.bin

cl:
	rm build/*.o
	rm build/Kernel.bin

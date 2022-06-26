SHELL=/bin/bash

GCCVER := 12.1.0

INCLUDE=~/gcc-$(GCCVER)-nolibc/i386-linux/lib/gcc/i386-linux/$(GCCVER)/include
CFLAGS := -I $(INCLUDE) -I ./include/ -Os -mno-mmx -mno-sse -mno-sse2 -mno-3dnow -mpreferred-stack-boundary=2 -nostartfiles -nodefaultlibs -ffreestanding -std=gnu99 -Wall

TCHAIN := ~/gcc-$(GCCVER)-nolibc/i386-linux/bin

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

KERNL386.EXE: $(OBJ)
	$(LINK) -T Kernel.ld $(addprefix build/, $(OBJ)) -o build/Kernel
	objcopy -O binary build/Kernel build/KERNL386.EXE

all: KERNL386.EXE

cl:
	rm build/*.o

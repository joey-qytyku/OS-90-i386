SHELL=/bin/bash

GCCVER := 10.1.0

TCHAIN := /usr/bin

#INCLUDE=~/gcc-$(GCCVER)-nolibc/i386-linux/lib/gcc/i386-linux/$(GCCVER)/include
CFLAGS := -I ./include/ -O2 -mno-mmx -mno-sse -mno-sse2 -mno-3dnow -mpreferred-stack-boundary=2 -march=i386 -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -std=gnu99 -Wall #-I $(INCLUDE)


CC=$(TCHAIN)/i586-pc-linux-gnu-gcc
LINK=$(TCHAIN)/i586-pc-linux-gnu-ld

CSOURCE=$(wildcard *.c)
ASOURCE=$(wildcard *.asm)

OBJ=$(patsubst %.c,%.c.o,$(wildcard *.c)) $(patsubst %.asm,%.asm.o,$(wildcard *.asm))

AS=nasm
ASFLAGS=-felf

%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o build/$@

%.asm.o: %.asm
	$(AS) $(ASFLAGS) $< -o build/$@

KERNL386.EXE: $(OBJ)
	echo Linking object files: [$(OBJ)]
	$(LINK) --oformat binary -T Kernel.ld $(addprefix build/, $(OBJ)) -o build/KERNL386.EXE

all: KERNL386.EXE

cl:
	rm build/*.o

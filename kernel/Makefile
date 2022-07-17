SHELL=/bin/bash

GCCVER := 10.1.0

INCLUDE=~/gcc-$(GCCVER)-nolibc/i386-linux/lib/gcc/i386-linux/$(GCCVER)/include
CFLAGS := -I $(INCLUDE) -I ./include/ -O2 -mno-mmx -mno-sse -mno-sse2 -mno-3dnow -mpreferred-stack-boundary=2 -march=i386 -nostartfiles -nodefaultlibs -ffreestanding -std=gnu99 -Wall

TCHAIN := ~/gcc-$(GCCVER)-nolibc/i386-linux/bin

CC=$(TCHAIN)/i386-linux-gcc
LINK=$(TCHAIN)/i386-linux-ld

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

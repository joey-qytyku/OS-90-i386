# Makefile supports CRLF and LF

# Change to wcc386 if not a beta build
# http://users.pja.edu.pl/~jms/qnx/help/watcom/compiler-tools/cpopts.html

CFLAGS=-m32 -mno-mmx -mno-sse -mno-sse2 -mno-3dnow
CC=bwcc386
AS=bwasm

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

kernel.bin: *.o
	ld -T kernel.ld *.o

all:kernel.bin

clean:all
	rm *.o


# Makefile supports CRLF and LF

define LINK
bwlink
name kernel.bin
	output raw
	offset=0
order
	clname CODE offset=0
	clname DATA
endef

# Change to wcc386 if not a beta build
# http://users.pja.edu.pl/~jms/qnx/help/watcom/compiler-tools/cpopts.html
DIRS=
CFLAGS=-wx\
-5\
-5r\
-ol\
-oi\
-mf\
-s

CC=bwcc386
AS=bwasm

%.o: %.c
	$(CC) $(CFLAGS) $< -fo=$@

kernel.bin: *.o
	$(LINK) $(foreach I, $(wildcard *.o), file $(I))

all:kernel.bin

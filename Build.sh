#!/bin/bash

set -e

echo "==| BUILDING BOOTLOADER |=="

cd boot
make all
cd ..

echo "==|   BUILDING KERNEL   |=="

cd kernel
make all
cd ..

# No need to use a loopback, I use DOSBox

dosbox

qemu-system-i386 -hda msdos.img\
    -m 4\
    -no-reboot\
    -no-shutdown

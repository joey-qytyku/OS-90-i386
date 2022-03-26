#!/bin/bash

USE_LOOP=$(losetup -f)

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

cd kernel
make clean
cd ..

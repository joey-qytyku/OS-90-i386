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

sudo losetup -o8704 $USE_LOOP msdos.img
sudo mount -t vfat $USE_LOOP img/

sudo umount $USE_LOOP
sudo losetup -d $USE_LOOP

cd kernel
make clean
cd ..
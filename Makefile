all:
	cd boot
	make
	cd ..

	cd kernel
	make
	cd ..

	losetup -o8704 {losetup -f} msdos.img



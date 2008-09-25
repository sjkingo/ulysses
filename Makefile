.PHONY: all kernel run gdb docs

all: kernel

kernel:
	$(MAKE) -C kernel kernel

run:
	qemu -s -S -fda boot/grub.img -hda fat:kernel -boot a -m 8

gdb:
	gdb -x gdb-startup

docs:
	rm -rf docs && doxygen

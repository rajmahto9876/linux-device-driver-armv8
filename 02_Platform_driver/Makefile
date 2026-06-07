obj-m := platform.o
platform-objs += main.o

ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-

#set KERN_DIR to linux source location (Beaglebone)
KERN_DIR = <linux_host>/linux

#set KERN_DIR to linux source location (Linux Ubuntu)
HOST_KERN_DIR = /lib/modules/$(shell uname -r)/build/

ccflags-y := -DMODULE_DEBUG

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help

dtbo:
	dtc -@ -I dts -O dtb -o qemu.dtbo qemu.dts

copy:
	cp -r ../02_platform_driver/ <qemu_host>/host_shared

copy_dtbo:
	cp *.dtbo  <qemu_files_dist>/qemu-rootfs

get_info:
	file platform.ko
	modinfo platform.ko

preprocess:
	make -C $(HOST_KERN_DIR) M=$(PWD)  platform.i


obj-m := hello.o
hello-objs += main.o

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

copy_ko:
	cp -r ../00_Basic/ <qemu_shared>/host_shared

get_info:
	file hello.ko
	modinfo hello.ko

preprocess:
	make -C $(HOST_KERN_DIR) M=$(PWD)  hello.i



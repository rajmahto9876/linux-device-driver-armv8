obj-m += irq_demo.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

# Fallback mode (no irq_num) -- works unmodified on QEMU
load:
	insmod irq_demo.ko

# Real hardware mode -- pass the irq number you obtained, see EXERCISES.md
load-irq:
	insmod irq_demo.ko irq_num=$(IRQ)

unload:
	rmmod irq_demo

.PHONY: all clean load load-irq unload

obj-m += drvbroken.o
ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-
# target kernel directory
KERN_DIR =~/ela_lab_exercises_rpi/rpi_build/kernel/linux/

# ubuntu kernel directory
HOST_KERN_DIR := /lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules
clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help
host:
	make -C $(HOST_KERN_DIR) M=$(PWD) modules

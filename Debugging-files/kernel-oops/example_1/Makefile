
# Default architecture
ARCH ?= x86_64

# Cross-compiler based on architecture
ifeq ($(ARCH), arm64)
	CROSS_COMPILE=aarch64-linux-gnu-
	KERN_DIR=${HOME}/ela_lab_exercises_rpi/rpi_build/kernel/linux/
else ifeq ($(ARCH), arm)
	CROSS_COMPILE=arm-linux-gnueabihf-
	KERN_DIR=${HOME}/ela_lab_exercises_bbb/bbb_build/kernel/linux/
else
	CROSS_COMPILE=
	KERN_DIR=/lib/modules/$(shell uname -r)/build/
endif

# Module name
obj-m += oops-test-app.o

# Targets
.PHONY: all clean help host

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) clean

help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERN_DIR) M=$(PWD) help

host:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

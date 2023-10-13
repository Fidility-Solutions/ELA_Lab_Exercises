obj-m += ldd_static_major_minor.o
obj-m += ldd_dynamic_major_minor.o

ifdef ARCH
	KDIR = /home/fidility/ela_lab_exercises/bbb-build/kernel/linux
else
  	KDIR = /lib/modules/$(shell uname -r)/build
endif
 
all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean

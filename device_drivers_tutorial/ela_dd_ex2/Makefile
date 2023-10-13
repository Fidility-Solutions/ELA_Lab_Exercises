obj-m += ldd_module_param.o

ifdef ARCH
	KDIR = /home/fidility/ela_lab_exercises/bbb-build/kernel/linux
else
  	KDIR = /lib/modules/$(shell uname -r)/build
endif
 
all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean

# This is a comment line
#CC=arm-buildroot-linux-gnueabihf-gcc
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
    endif
    ifeq ($(ARCH),aarch64)
        CC := aarch64-linux-gnu-gcc
    endif
endif

.PHONY: clean

# Explicit Rule
all: filelocking_fcntl

filelocking_fcntl:filelocking_fcntl.c
	$(CC) -o filelocking_fcntl filelocking_fcntl.c


clean:
	rm -rf *.o filelocking_fcntl

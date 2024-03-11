# This is a comment line
#CC=arm-buildroot-linux-gnueabihf-gcc
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
        CFLAGS += -static
    endif
    ifeq ($(ARCH),aarch64)
        CC := aarch64-linux-gnu-gcc
        CFLAGS += -static
    endif
endif
#CC=arm-buildroot-linux-gnueabihf-gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: Child_Process_execl.c  
#	$(CC)  -static Child_Process.c -o Child_Process

# Implicit Rule
all: Child_Process_execl.o
	$(CC)  Child_Process_execl.o -o Child_Process_execl

clean:
	rm -rf *.o ./Child_Process_execl

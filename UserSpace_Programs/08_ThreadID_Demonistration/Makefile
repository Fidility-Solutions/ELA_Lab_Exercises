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
all: Thread_IDs.c  
#	$(CC)  -static Thread_IDs.c -o Thread_IDs

# Implicit Rule
all: Thread_IDs.o
	$(CC)  Thread_IDs.o -o Thread_IDs

clean:
	rm -rf *.o ./Thread_IDs

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
all: Zombie_Process.c  
#	$(CC)  -static Zombie_Process.c -o Zombie_Process

# Implicit Rule
all: Zombie_Process.o
	$(CC)  Zombie_Process.o -o Zombie_Process

clean:
	rm -rf *.o ./Zombie_Process

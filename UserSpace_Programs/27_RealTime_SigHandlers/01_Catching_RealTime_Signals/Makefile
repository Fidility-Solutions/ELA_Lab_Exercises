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
all: Catch_RtSignals SigQueue

Catch_RtSignals:Catch_RtSignals.c
	$(CC) $(FLAGS) -o Catch_RtSignals Catch_RtSignals.c

SigQueue:SigQueue.c
	$(CC) $(FLAGS) -o SigQueue SigQueue.c


clean:
	rm -rf *.o Catch_RtSignals SigQueue 


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
all:sig_receiver sig_sender

sig_receiver:sig_receiver.c
	$(CC) $(FLAGS) -o sig_receiver sig_receiver.c

sig_sender:sig_sender.c
	$(CC) $(FLAGS) -o sig_sender sig_sender.c
clean:
	rm -rf *.o sig_receiver sig_sender

# This is a comment line
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
all:sig_receiver sig_sender

sig_receiver:sig_receiver.c
	$(CC) -o sig_receiver sig_receiver.c

sig_sender:sig_sender.c
	$(CC) -o sig_sender sig_sender.c
clean:
	rm -rf *.o sig_receiver sig_sender

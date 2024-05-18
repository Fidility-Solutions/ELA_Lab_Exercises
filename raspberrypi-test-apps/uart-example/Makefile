# This is a comment line
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
        CFLAGS += -static
    endif
    ifeq ($(ARCH),arm64)
        CC := aarch64-linux-gnu-gcc
        CFLAGS += -static
    endif
endif

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: uart-echo-target uart-receive-target

uart-echo-target:uart-echo-target.c
	$(CC) $(FLAGS) -o uart-echo-target uart-echo-target.c

uart-receive-target:uart-receive-target.c
	$(CC) $(FLAGS) -o uart-receive-target uart-receive-target.c

clean:
	rm -rf *.o uart-echo-target uart-receive-target uart-transmit-host

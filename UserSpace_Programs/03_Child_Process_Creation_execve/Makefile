# This is a comment line
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

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: child_process_execve.c  
#	$(CC)  -static child_process_execve.c -o child_process_execve

# Implicit Rule
all: child_process_execve.o
	$(CC)  child_process_execve.o -o child_process_execve

clean:
	rm -rf *.o ./child_process_execve

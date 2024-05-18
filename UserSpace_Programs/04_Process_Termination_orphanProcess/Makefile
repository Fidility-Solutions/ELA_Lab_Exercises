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
all: Orphan_Process.c  
#	$(CC)  -static Orphan_Process.c -o Child_Process

# Implicit Rule
all: Orphan_Process.o
	$(CC)  Orphan_Process.o -o Orphan_Process

clean:
	rm -rf *.o ./Orphan_Process

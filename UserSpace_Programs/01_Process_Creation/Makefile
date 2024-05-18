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
all: Process_Creation.c  
#	$(CC)  -static Process_Creation.c -o Process_Creation

# Implicit Rule
all: Process_Creation.o
	$(CC)  Process_Creation.o -o Process_Creation

clean:
	rm -rf *.o ./Process_Creation

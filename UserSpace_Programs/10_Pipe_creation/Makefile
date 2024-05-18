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
all: pipe_creation.c  
#	$(CC)  -static pipe_creation.c -o pipe_creation

# Implicit Rule
all: pipe_creation.o
	$(CC)  pipe_creation.o -o pipe_creation

clean:
	rm -rf *.o ./pipe_creation

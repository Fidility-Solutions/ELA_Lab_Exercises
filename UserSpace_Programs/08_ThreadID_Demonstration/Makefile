# This is a comment line
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
        CFLAGS += -c -Wall
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
all: Thread_IDs

Thread_IDs:Thread_IDs.o
	$(CC) $(FLAGS) -pthread Thread_IDs.o -o Thread_IDs

Thread_IDs.o:Thread_IDs.c
	$(CC) $(FLAGS) -c Thread_IDs.c

clean:
	rm -rf ./Thread_IDs Thread_IDs.o

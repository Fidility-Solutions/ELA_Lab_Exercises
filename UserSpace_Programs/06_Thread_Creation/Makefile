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
all: Thread_Creation

Thread_Creation:Thread_Creation.o
	$(CC) $(FLAGS) -pthread Thread_Creation.o -o Thread_Creation

Thread_Creation.o:Thread_Creation.c
	$(CC) $(FLAGS) -c Thread_Creation.c

clean:
	rm -f ./Thread_Creation Thread_Creation.o

# This is a comment line
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
    endif
    ifeq ($(ARCH),arm64)
        CC := aarch64-linux-gnu-gcc
    endif
endif

.PHONY: clean

# Explicit Rule
all: Thread_Termination

Thread_Termination:Thread_Termination.o
	$(CC) -pthread Thread_Termination.o -o Thread_Termination

Thread_Termination.o:Thread_Termination.c
	$(CC) -c Thread_Termination.c

clean:
	rm -f ./Thread_Termination Thread_Termination.o

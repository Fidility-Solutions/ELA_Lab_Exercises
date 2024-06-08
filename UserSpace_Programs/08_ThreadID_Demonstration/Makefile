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
all: Thread_IDs

Thread_IDs:Thread_IDs.o
	$(CC) -pthread Thread_IDs.o -o Thread_IDs

Thread_IDs.o:Thread_IDs.c
	$(CC) -c Thread_IDs.c

clean:
	rm -rf ./Thread_IDs Thread_IDs.o

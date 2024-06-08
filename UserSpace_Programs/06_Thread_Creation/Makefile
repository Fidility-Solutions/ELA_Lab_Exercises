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
all: Thread_Creation

Thread_Creation:Thread_Creation.o
	$(CC) -pthread Thread_Creation.o -o Thread_Creation

Thread_Creation.o:Thread_Creation.c
	$(CC) -c Thread_Creation.c

clean:
	rm -f ./Thread_Creation Thread_Creation.o

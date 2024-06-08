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
all: Thread_Attributes

Thread_Attributes:Thread_Attributes.o
	$(CC) -pthread Thread_Attributes.o -o Thread_Attributes

Thread_Attributes.o:Thread_Attributes.c
	$(CC) -c Thread_Attributes.c

clean:
	rm -rf ./Thread_Attributes Thread_Attributes.o

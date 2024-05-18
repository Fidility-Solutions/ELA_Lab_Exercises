# This is a comment line
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
        CFLAGS += -static
    endif
    ifeq ($(ARCH),arm64)
        CC := aarch64-linux-gnu-gcc
        CFLAGS += -static
    endif
endif

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean
all: gpio-out

gpio-out:gpio-out.o
	$(CC) $(FLAGS) gpio-out.o -o gpio-out

gpio-out.o:gpio-out.c
	$(CC) $(FLAGS) -c gpio-out.c


clean:
	rm -rf ./gpio-out gpio-out.o


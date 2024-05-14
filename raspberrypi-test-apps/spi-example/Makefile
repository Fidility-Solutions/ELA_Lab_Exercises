# This is a comment line
#CC=arm-buildroot-linux-gnueabihf-gcc
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
#CC=arm-buildroot-linux-gnueabihf-gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: spi-flash.c  
#	$(CC)  -static spi-flash.c -o spi-flash

# Implicit Rule
all: spi-flash.o
	$(CC)  $(FLAGS) spi-flash.o -o spi-flash

clean:
	rm -rf *.o ./spi-flash spi-flash.o

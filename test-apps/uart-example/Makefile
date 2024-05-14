# This is a comment line
#CC=arm-buildroot-linux-gnueabihf-gcc
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
#CC=arm-buildroot-linux-gnueabihf-gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: uart-echo-target Uart_Receive

uart-echo-target:uart-echo-target.c
	$(CC) $(FLAGS) -o uart-echo-target uart-echo-target.c

Uart_Receive:Uart_Receive.c
	$(CC) $(FLAGS) -o Uart_Receive Uart_Receive.c

clean:
	rm -rf *.o uart-echo-target Uart_Receive

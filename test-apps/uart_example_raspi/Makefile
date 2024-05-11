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
all: uart_echo_target Uart_Receive_Target

uart_echo_target:uart_echo_target.c
	$(CC) $(FLAGS) -o uart_echo_target uart_echo_target.c

Uart_Receive_Target:Uart_Receive_Target.c
	$(CC) $(FLAGS) -o Uart_Receive_Target Uart_Receive_Target.c

clean:
	rm -rf *.o uart_echo_target Uart_Receive_Target

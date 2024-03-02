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
all: Dserver_socket Dclient_socket

Dserver_socket:Dserver_socket.c
	$(CC) $(FLAGS) -o Dserver_socket Dserver_socket.c

Dclient_socket:Dclient_socket.c
	$(CC) $(FLAGS) -o Dclient_socket Dclient_socket.c

clean:
	rm -rf *.o Dserver_socket Dclient_socket

# This is a comment line
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

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: Unix_Datagram_ServerSocket Unix_Datagram_ClientSocket

Unix_Datagram_ServerSocket:Unix_Datagram_ServerSocket.c
	$(CC) $(FLAGS) -o Unix_Datagram_ServerSocket Unix_Datagram_ServerSocket.c

Unix_Datagram_ClientSocket:Unix_Datagram_ClientSocket.c
	$(CC) $(FLAGS) -o Unix_Datagram_ClientSocket Unix_Datagram_ClientSocket.c

clean:
	rm -rf *.o Unix_Datagram_ServerSocket Unix_Datagram_ClientSocket

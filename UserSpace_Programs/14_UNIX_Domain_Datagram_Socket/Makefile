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
all: Unix_Datagram_ServerSocket Unix_Datagram_ClientSocket

Unix_Datagram_ServerSocket:Unix_Datagram_ServerSocket.c
	$(CC) -o Unix_Datagram_ServerSocket Unix_Datagram_ServerSocket.c

Unix_Datagram_ClientSocket:Unix_Datagram_ClientSocket.c
	$(CC) -o Unix_Datagram_ClientSocket Unix_Datagram_ClientSocket.c

clean:
	rm -rf *.o Unix_Datagram_ServerSocket Unix_Datagram_ClientSocket

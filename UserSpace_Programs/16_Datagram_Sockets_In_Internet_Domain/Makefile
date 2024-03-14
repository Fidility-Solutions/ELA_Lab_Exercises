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
all: Iserver_socket Iclient_socket Internet_Datagram_IPV4_SrvrSkt Internet_Datagram_IPV4_ClntSkt

Iserver_socket:Iserver_socket.c
	$(CC) $(FLAGS) -o Iserver_socket Iserver_socket.c

Iclient_socket:Iclient_socket.c
	$(CC) $(FLAGS) -o Iclient_socket Iclient_socket.c

Internet_Datagram_IPV4_SrvrSkt:Internet_Datagram_IPV4_SrvrSkt.c
	$(CC) $(FLAGS) -o Internet_Datagram_IPV4_SrvrSkt Internet_Datagram_IPV4_SrvrSkt.c

Internet_Datagram_IPV4_ClntSkt:Internet_Datagram_IPV4_ClntSkt.c
	$(CC) $(FLAGS) -o Internet_Datagram_IPV4_ClntSkt Internet_Datagram_IPV4_ClntSkt.c

clean:
	rm -rf *.o Iserver_socket Iclient_socket Internet_Datagram_IPV4_SrvrSkt Internet_Datagram_IPV4_ClntSkt

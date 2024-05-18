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
all: Internet_Datagram_IPV4_SrvrSkt Internet_Datagram_IPV4_ClntSkt

Internet_Datagram_IPV4_SrvrSkt:Internet_Datagram_IPV4_SrvrSkt.c
	$(CC) $(FLAGS) -o Internet_Datagram_IPV4_SrvrSkt Internet_Datagram_IPV4_SrvrSkt.c

Internet_Datagram_IPV4_ClntSkt:Internet_Datagram_IPV4_ClntSkt.c
	$(CC) $(FLAGS) -o Internet_Datagram_IPV4_ClntSkt Internet_Datagram_IPV4_ClntSkt.c

clean:
	rm -rf *.o Internet_Datagram_IPV4_SrvrSkt Internet_Datagram_IPV4_ClntSkt

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
all: UNIX_Stream_SrvrSkt UNIX_Stream_ClntSkt

UNIX_Stream_SrvrSkt:UNIX_Stream_SrvrSkt.c
	$(CC) $(FLAGS) -o UNIX_Stream_SrvrSkt UNIX_Stream_SrvrSkt.c

UNIX_Stream_ClntSkt:UNIX_Stream_ClntSkt.c
	$(CC) $(FLAGS) -o UNIX_Stream_ClntSkt UNIX_Stream_ClntSkt.c

clean:
	rm -rf *.o UNIX_Stream_SrvrSkt UNIX_Stream_ClntSkt

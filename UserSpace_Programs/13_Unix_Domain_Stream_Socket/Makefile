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
all: UNIX_Stream_SrvrSkt UNIX_Stream_ClntSkt

UNIX_Stream_SrvrSkt:UNIX_Stream_SrvrSkt.c
	$(CC) -o UNIX_Stream_SrvrSkt UNIX_Stream_SrvrSkt.c

UNIX_Stream_ClntSkt:UNIX_Stream_ClntSkt.c
	$(CC) -o UNIX_Stream_ClntSkt UNIX_Stream_ClntSkt.c

clean:
	rm -rf *.o UNIX_Stream_SrvrSkt UNIX_Stream_ClntSkt

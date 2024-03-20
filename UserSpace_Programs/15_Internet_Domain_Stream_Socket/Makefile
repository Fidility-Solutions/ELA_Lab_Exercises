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
all: Internet_Stream_Server_Socket Internet_Stream_Client_Socket

Internet_Stream_Server_Socket:Internet_Stream_Server_Socket.c
	$(CC) $(FLAGS) -o Internet_Stream_Server_Socket Internet_Stream_Server_Socket.c

Internet_Stream_Client_Socket:Internet_Stream_Client_Socket.c
	$(CC) $(FLAGS) -o Internet_Stream_Client_Socket Internet_Stream_Client_Socket.c

clean:
	rm -rf *.o Internet_Stream_Server_Socket Internet_Stream_Client_Socket

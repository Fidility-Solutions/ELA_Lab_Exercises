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
all: FIFO server_fifo client_fifo

FIFO:FIFO.c
	$(CC) $(FLAGS) -o FIFO FIFO.c

server_fifo:server_fifo.c
	$(CC) $(FLAGS) -o server_fifo server_fifo.c

client_fifo:client_fifo.c
	$(CC) $(FLAGS) -o client_fifo client_fifo.c

clean:
	rm -rf *.o ./FIFO myfifo server_fifo client_fifo

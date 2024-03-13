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

# Specify the targets to be built
TARGETS = FIFO server_fifo client_fifo
.PHONY: clean

# Explicit Rule
all: $(TARGETS)

FIFO:FIFO.c
	$(CC) $(FLAGS) -o FIFO FIFO.c

server_fifo:server_fifo.c
	$(CC) $(FLAGS) -o server_fifo server_fifo.c

client_fifo:client_fifo.c
	$(CC) $(FLAGS) -o client_fifo client_fifo.c


#clean:
#	rm -rf *.o ./FIFO myfifo server_fifo client_fifo

# Rule to clean up generated files
clean:
	rm -f *.o $(TARGETS)

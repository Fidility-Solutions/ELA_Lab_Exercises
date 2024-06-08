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

# Specify the targets to be built
TARGETS = Simple_Fifo server_fifo client_fifo
.PHONY: clean

# Explicit Rule
all: $(TARGETS)

Simple_Fifo:Simple_Fifo.c
	$(CC) -o Simple_Fifo Simple_Fifo.c

server_fifo:server_fifo.c
	$(CC) -o server_fifo server_fifo.c

client_fifo:client_fifo.c
	$(CC) -o client_fifo client_fifo.c

# Rule to clean up generated files
clean:
	rm -f *.o $(TARGETS)

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
all: shared_memory_writer shared_memory_reader

shared_memory_writer:shared_memory_writer.c
	$(CC) $(FLAGS) -o shared_memory_writer shared_memory_writer.c

shared_memory_reader:shared_memory_reader.c
	$(CC) $(FLAGS) -o shared_memory_reader shared_memory_reader.c

clean:
	rm -rf *.o shared_memory_reader shared_memory_writer

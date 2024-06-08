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
all: shared_memory_writer shared_memory_reader

shared_memory_writer:shared_memory_writer.c
	$(CC) -o shared_memory_writer shared_memory_writer.c -lrt

shared_memory_reader:shared_memory_reader.c
	$(CC) -o shared_memory_reader shared_memory_reader.c -lrt

clean:
	rm -rf *.o shared_memory_reader shared_memory_writer

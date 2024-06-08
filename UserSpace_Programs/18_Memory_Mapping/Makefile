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
all: private_memory_mapping shared_memory_mapping

private_memory_mapping:private_memory_mapping.c
	$(CC) -o private_memory_mapping private_memory_mapping.c

shared_memory_mapping:shared_memory_mapping.c
	$(CC) -o shared_memory_mapping shared_memory_mapping.c

clean:
	rm -rf *.o shared_memory_mapping private_memory_mapping

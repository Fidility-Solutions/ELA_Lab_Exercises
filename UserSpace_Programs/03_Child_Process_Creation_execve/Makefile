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
all: print_data child_process_execve

print_data:print_data.c
	$(CC) -o print_data print_data.c

child_process_execve:child_process_execve.c
	$(CC) -o child_process_execve child_process_execve.c

clean:
	rm -rf *.o ./child_process_execve print_data

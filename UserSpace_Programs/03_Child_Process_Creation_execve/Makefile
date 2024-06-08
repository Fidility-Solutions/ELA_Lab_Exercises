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

# Implicit Rule
all: child_process_execve.o
	$(CC)  child_process_execve.o -o child_process_execve

clean:
	rm -rf *.o ./child_process_execve

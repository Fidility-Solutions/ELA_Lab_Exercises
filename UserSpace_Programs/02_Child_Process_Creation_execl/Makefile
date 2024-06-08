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
all: Child_Process_execl.o
	$(CC)  Child_Process_execl.o -o Child_Process_execl

clean:
	rm -rf *.o ./Child_Process_execl

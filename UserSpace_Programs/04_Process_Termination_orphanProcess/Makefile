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
all: Orphan_Process.o
	$(CC)  Orphan_Process.o -o Orphan_Process

clean:
	rm -rf *.o ./Orphan_Process

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
all: Zombie_Process.o
	$(CC)  Zombie_Process.o -o Zombie_Process

clean:
	rm -rf *.o ./Zombie_Process

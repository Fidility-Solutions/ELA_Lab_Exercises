# This is a comment line
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
    endif
    ifeq ($(ARCH),aarch64)
        CC := aarch64-linux-gnu-gcc
    endif
endif

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: oom_killer.c  
#	$(CC)  -static oom_killer.c -o oom_killer

# Implicit Rule
all: oom_killer.o
	$(CC)  oom_killer.o -o oom_killer

clean:
	rm -rf *.o ./oom_killer

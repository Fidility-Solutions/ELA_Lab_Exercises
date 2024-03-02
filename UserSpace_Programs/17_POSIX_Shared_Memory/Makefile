# This is a comment line
#CC=arm-buildroot-linux-gnueabihf-gcc
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
#CC=arm-buildroot-linux-gnueabihf-gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: Shm_WritterProcess Shm_ReaderProcess

Shm_WritterProcess:Shm_WritterProcess.c
	$(CC) $(FLAGS) -o Shm_WritterProcess Shm_WritterProcess.c

Shm_ReaderProcess:Shm_ReaderProcess.c
	$(CC) $(FLAGS) -o Shm_ReaderProcess Shm_ReaderProcess.c

clean:
	rm -rf *.o Shm_ReaderProcess Shm_WritterProcess

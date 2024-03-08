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
all: named_semaphore_using_process unnamed_semaphore named_semaphore_using_thread

named_semaphore_using_process:named_semaphore_using_process.c
	$(CC) $(FLAGS) -o named_semaphore_using_process named_semaphore_using_process.c

unnamed_semaphore:unnamed_semaphore.c
	$(CC) $(FLAGS) -o unnamed_semaphore unnamed_semaphore.c

named_semaphore_using_thread:named_semaphore_using_thread.c
	$(CC) $(FLAGS) -o named_semaphore_using_thread named_semaphore_using_thread.c

clean:
	rm -rf *.o named_semaphore_using_process unnamed_semaphore named_semaphore_using_thread

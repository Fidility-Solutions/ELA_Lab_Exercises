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
all: named_semaphore_using_process unnamed_semaphore named_semaphore_using_thread

named_semaphore_using_process:named_semaphore_using_process.c
	$(CC) -o named_semaphore_using_process named_semaphore_using_process.c

unnamed_semaphore:unnamed_semaphore.c
	$(CC) -o unnamed_semaphore unnamed_semaphore.c

named_semaphore_using_thread:named_semaphore_using_thread.c
	$(CC) -o named_semaphore_using_thread named_semaphore_using_thread.c

clean:
	rm -rf *.o named_semaphore_using_process unnamed_semaphore named_semaphore_using_thread

# This is a comment line
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
	LDFLAGS := -pthread -lrt
    endif
    ifeq ($(ARCH),arm64)
        CC := aarch64-linux-gnu-gcc
	LDFLAGS := -pthread -lrt
    endif
endif
# If ARCH is not defined, use the default compiler
ifndef CC
    CC := gcc
    LDFLAGS := -pthread -lrt
endif

.PHONY: clean

# Explicit Rule
all: named_semaphore_using_process unnamed_semaphore named_semaphore_using_thread

named_semaphore_using_process:named_semaphore_using_process.c
	$(CC) -o named_semaphore_using_process named_semaphore_using_process.c $(LDFLAGS)

unnamed_semaphore:unnamed_semaphore.c
	$(CC) -o unnamed_semaphore unnamed_semaphore.c $(LDFLAGS)

named_semaphore_using_thread:named_semaphore_using_thread.c
	$(CC) -o named_semaphore_using_thread named_semaphore_using_thread.c $(LDFLAGS)

clean:
	rm -rf *.o named_semaphore_using_process unnamed_semaphore named_semaphore_using_thread

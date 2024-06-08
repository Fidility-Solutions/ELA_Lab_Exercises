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
all: pthread_mutex_static pthread_mutex_dynamic

pthread_mutex_static:pthread_mutex_static.c
	$(CC) -o pthread_mutex_static pthread_mutex_static.c


pthread_mutex_dynamic:pthread_mutex_dynamic.c
	$(CC) -o pthread_mutex_dynamic pthread_mutex_dynamic.c

clean:
	rm -rf *.o pthread_mutex_dynamic pthread_mutex_static

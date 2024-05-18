# This is a comment line
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

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: pthread_mutex_static pthread_mutex_dynamic

pthread_mutex_static:pthread_mutex_static.c
	$(CC) $(FLAGS) -o pthread_mutex_static pthread_mutex_static.c


pthread_mutex_dynamic:pthread_mutex_dynamic.c
	$(CC) $(FLAGS) -o pthread_mutex_dynamic pthread_mutex_dynamic.c

clean:
	rm -rf *.o pthread_mutex_dynamic pthread_mutex_static

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
all: condition_variable_static condition_variable_dynamic

condition_variable_static:condition_variable_static.c
	$(CC) $(FLAGS) -o condition_variable_static condition_variable_static.c

condition_variable_dynamic:condition_variable_dynamic.c
	$(CC) $(FLAGS) -o condition_variable_dynamic condition_variable_dynamic.c

clean:
	rm -rf *.o condition_variable_dynamic condition_variable_static

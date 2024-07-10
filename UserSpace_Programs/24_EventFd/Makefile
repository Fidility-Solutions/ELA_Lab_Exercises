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
CFLAGS= -c -Wall -Wextra -pthread

.PHONY: clean

# Explicit Rule
all: Event_Fd 

Event_Fd:Event_Fd.c
	$(CC) $(FLAGS) -o Event_Fd Event_Fd.c

clean:
	rm -rf *.o Event_Fd 

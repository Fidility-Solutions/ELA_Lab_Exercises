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
all: RT_SigHndlr RT_Signals

RT_SigHndlr:RT_SigHndlr.c
	$(CC) $(FLAGS) -o RT_SigHndlr RT_SigHndlr.c

RT_Signals:RT_Signals.c
	$(CC) $(FLAGS) -o RT_Signals RT_Signals.c

clean:
	rm -rf *.o RT_SigHndlr RT_Signals

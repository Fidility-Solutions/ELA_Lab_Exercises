# This is a comment line
CC=arm-buildroot-linux-gnueabihf-gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

# Explicit Rule
all: helloworld.c  
#	$(CC)  -static helloworld.c -o helloworld

# Implicit Rule
all: helloworld.o
	$(CC)  helloworld.o -o helloworld

clean:
	rm -rf *.o ./helloworld 

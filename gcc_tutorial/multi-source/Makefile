# This is a comment line
CC=gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean

all:  addsub 
addsub: main.o add.o sub.o 
	$(CC)  main.o add.o sub.o -o addsub

main.o: main.c
	@$(CC) $(CFLAGS) main.c

add.o: add.c
	@$(CC) $(CFLAGS) add.c

sub.o: sub.c
	@$(CC) $(CFLAGS) sub.c


clean:
	rm -rf *.o ./addsub ./install_dir

install:
	mkdir install_dir
	cp ./addsub ./install_dir

# This is a comment line
#CC=arm-buildroot-linux-gnueabihf-gcc
# Determine the compiler based on the ARCH variable
ifdef ARCH
    ifeq ($(ARCH),arm)
        CC := arm-linux-gnueabihf-gcc
        CFLAGS += -static
    endif
    ifeq ($(ARCH),arm64)
        CC := aarch64-linux-gnu-gcc
        CFLAGS += -static
    endif
endif
#CC=arm-buildroot-linux-gnueabihf-gcc

# CFLAGS will be the options passed to the compiler.
CFLAGS= -c -Wall

.PHONY: clean
all: i2c-eeprom-read i2c-eeprom-read-write

i2c-eeprom-read:i2c-eeprom-read.o
	$(CC) $(FLAGS) i2c-eeprom-read.o -o i2c-eeprom-read

i2c-eeprom-read.o:i2c-eeprom-read.c
	$(CC) $(FLAGS) -c i2c-eeprom-read.c


i2c-eeprom-read-write:i2c-eeprom-read-write.o
	$(CC) $(FLAGS) i2c-eeprom-read-write.o -o i2c-eeprom-read-write

i2c-eeprom-read-write.o:i2c-eeprom-read-write.c
	$(CC) $(FLAGS) -c i2c-eeprom-read-write.c

clean:
	rm -rf ./i2c-eeprom-read-write i2c-eeprom-read-write.o i2c-eeprom-read i2c-eeprom-read.o


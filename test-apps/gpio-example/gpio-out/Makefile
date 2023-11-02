# Copyright (C) 2017, Chris Simmonds (chris@2net.co.uk)
#
# If cross-compiling, CC must point to your cross compiler, for example:
CC=arm-linux-gnueabihf-gcc

LOCAL_CFLAGS ?= -Wall -g
PROGRAM = gpio-out

$(PROGRAM): $(PROGRAM).c
	$(CC) $(CFLAGS) $(LOCAL_CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(PROGRAM)


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

all:oom-killer-test-app.o
	$(CC) oom-killer-test-app.o -o oom-killer-test-app

clean:
	rm -rf *.o oom-killer-test-app

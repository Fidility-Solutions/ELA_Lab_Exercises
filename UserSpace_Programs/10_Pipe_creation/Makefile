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
all: simple_pipe pipe_using_dup

simple_pipe:simple_pipe.c
	$(CC) -o simple_pipe simple_pipe.c

pipe_using_dup:pipe_using_dup.c
	$(CC) -o pipe_using_dup pipe_using_dup.c

clean:
	rm -rf *.o pipe_using_dup simple_pipe


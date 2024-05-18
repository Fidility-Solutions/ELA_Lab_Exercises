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
all: Message_Queue User_MsgQueue

Message_Queue:Message_Queue.c
	$(CC) $(FLAGS) -o Message_Queue Message_Queue.c

User_MsgQueue:User_MsgQueue.c
	$(CC) $(FLAGS) -o User_MsgQueue User_MsgQueue.c

clean:
	rm -rf *.o Message_Queue User_MsgQueue

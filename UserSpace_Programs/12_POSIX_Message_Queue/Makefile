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
all: Message_Queue User_MsgQueue

Message_Queue:Message_Queue.c
	$(CC) -o Message_Queue Message_Queue.c -lrt

User_MsgQueue:User_MsgQueue.c
	$(CC) -o User_MsgQueue User_MsgQueue.c -lrt

clean:
	rm -rf *.o Message_Queue User_MsgQueue

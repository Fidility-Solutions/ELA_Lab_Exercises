
# Define the compilers
# BBB
CC_ARM = arm-linux-gnu-gcc

#Raspberry Pi
CC_ARM64 = aarch64-linux-gnu-gcc

# Native
CC_NATIVE = gcc

# Define the source files
SRC_ARM		= Internet_Stream_Server_Socket.c
SRC_NATIVE	= Internet_Stream_Client_Socket.c

# Define the output binaries
BIN_ARM     = Internet_Stream_Server_Socket
BIN_NATIVE  = Internet_Stream_Client_Socket

# Define the architecture
ifdef ARCH
# Select the compiler and binaries based on the architecture
ifeq ($(ARCH), arm)
    CC = $(CC_ARM)
endif
ifeq ($(ARCH), arm64)
    CC = $(CC_ARM64)
endif
endif

# Default target
all: $(BIN_ARM) $(BIN_NATIVE)

# Rule for compiling the ARM/ARM64/native file
$(BIN_ARM): $(SRC_ARM)
	$(CC) -o $@ $<

# Rule for compiling the shared file
$(BIN_NATIVE): $(SRC_NATIVE)
	$(CC_NATIVE) -o $@ $<

# Clean up the generated files
clean:
	rm -rf *.o $(BIN_ARM) $(BIN_NATIVE)


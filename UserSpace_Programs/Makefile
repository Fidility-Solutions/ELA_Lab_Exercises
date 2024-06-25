# Determine the platform
UNAME := $(shell uname -m)

# Raspberry Pi (ARM)
ifeq ($(ARCH), arm64)
    CC := aarch64-linux-gnu-gcc
    CFLAGS := -Wall -Wextra
    LDFLAGS := -pthread -lrt
# BeagleBone Black (ARM)
else ifeq ($(ARCH), arm)
    CC := arm-linux-gnueabihf-gcc
    CFLAGS := -Wall -Wextra
    LDFLAGS := -pthread -lrt
# x86 architecture (Linux)
else ifeq ($(UNAME), x86_64)
    CC := gcc
    CFLAGS := -Wall -Wextra
    LDFLAGS := -pthread
else
    $(error Unsupported platform: $(UNAME))
endif

# List of subdirectories containing individual Makefiles
SUBDIRS := \
    01_Process_Creation \
    02_Child_Process_Creation_execl \
    03_Child_Process_Creation_execve \
    04_Process_Termination_orphanProcess \
    05_Process_Termination_zombieProcess \
    06_Thread_Creation \
    07_Thread_Termination \
    08_ThreadID_Demonstration \
    09_Thread_with_Atrributes \
    10_Pipe_creation \
    11_FIFO \
    12_POSIX_Message_Queue \
    13_Unix_Domain_Stream_Socket \
    14_UNIX_Domain_Datagram_Socket \
    15_Internet_Domain_Stream_Socket \
    16_Internet_Domain_Datagram_Socket \
    17_POSIX_Shared_Memory \
    18_Memory_Mapping \
    19_POSIX_Semaphore \
    20_File_Locking_using_flock \
    21_File_Locking_using_fcntl \
    22_Mutual_Exclusion \
    23_Condition_Variable \
    24_Standard_SigHandler \
    25_Catching_Counting_StandardSignals

# Targets
.PHONY: all clean

# Default target: build all programs
all:
	@for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir; \
    done

# Clean target: remove all executables
clean:
	@for dir in $(SUBDIRS); do \
        $(MAKE) -C $$dir clean; \
    done


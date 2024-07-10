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
all:SIG_BLK_UNBLK SendSig_BtwnProcess

SIG_BLK_UNBLK:01_SIG_BLK_UNBLK.c
	$(CC) -o SIG_BLK_UNBLK 01_SIG_BLK_UNBLK.c

SendSig_BtwnProcess:02_SendSig_BtwnProcess.c
	$(CC) -o SendSig_BtwnProcess 02_SendSig_BtwnProcess.c

clean:
	rm -rf *.o SIG_BLK_UNBLK SendSig_BtwnProcess

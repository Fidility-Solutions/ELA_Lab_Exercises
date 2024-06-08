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
all: Static_Condition_Variable Dynamic_Condition_Variable

Static_Condition_Variable:Static_Condition_Variable.c
	$(CC) -o Static_Condition_Variable Static_Condition_Variable.c

Dynamic_Condition_Variable:Dynamic_Condition_Variable.c
	$(CC) -o Dynamic_Condition_Variable Dynamic_Condition_Variable.c

clean:
	rm -rf *.o Dynamic_Condition_Variable Static_Condition_Variable

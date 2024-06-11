CC=${CROSS_COMPILE}gcc

all:allocate_memory

allocate_memory: allocate_memory.c
	${CC} $< -g3 -o $@

clean:
	rm -f *.o  allocate_memory

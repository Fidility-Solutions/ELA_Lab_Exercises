CC=${CROSS_COMPILE}gcc
all:array_core crash_core
array_core: array_core.c
	${CC} $< -g3 -o $@

crash_core:crash_core.c
	${CC} $< -g3 -o $@

clean:
	rm -rf *.o array_core crash_core

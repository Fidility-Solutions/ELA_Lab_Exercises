CC=${CROSS_COMPILE}gcc

all:file_reader

file_reader: file_reader.c
	${CC} $< -g3 -o $@


clean:
	rm -f *.o  file_reader.c

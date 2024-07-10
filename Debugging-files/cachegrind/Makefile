CC=${CROSS_COMPILE}gcc

all:linked_list

linked_list: linked_list.c
	${CC} $< -g3 -o $@

clean:
	rm -f *.o  linked_list

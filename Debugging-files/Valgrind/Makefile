CC=${CROSS_COMPILE}gcc

faulty_mem_app: faulty_mem_app.c
	${CC} $< -g3 -o $@

clean:
	rm -rf *.o faulty_mem_app

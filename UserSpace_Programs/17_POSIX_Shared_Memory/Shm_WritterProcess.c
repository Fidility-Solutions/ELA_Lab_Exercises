
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SHARED_MEM_SIZE 1024
#define SHARED_MEM_NAME "my_shared_memory"

int main() {
	printf("\n\nYou have entered into Shared Memory writter Process!\n");
	printf("Welcome to POSIX Shared Memory writter Program\n");
	printf("The Writter Process id(PID):%d\n",getpid());
    	/* Open existing shared memory object */
	printf("Trying to open shared memory region...!\n");
    	int shm_fd = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    	if (shm_fd == -1) {
        	perror("shm_open error");
        	exit(EXIT_FAILURE);
    	}

    	/* Map the shared memory object into the address space */
	printf("Mapping the shared memory object into the address space...\n"); 
    	char *child_shared_memory = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    	if (child_shared_memory == MAP_FAILED) {
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}
	
	/* write data to shared memory object */
    	printf("Writing data to shared memory object...\n");
    	sprintf(child_shared_memory, "Hello from Shared Memory Writter Process");
    	printf("The Shared Memory Writter Process wrote: %s\n", child_shared_memory);

    	/* Unmap shared memory in the child process */
	printf("\nUnmapping the shared memory object...\n");
    	if (munmap(child_shared_memory, SHARED_MEM_SIZE) == -1) {
        	perror("munmap");
        	exit(EXIT_FAILURE);
    	}

    	/* Close shared memory object */
	printf("The shared memory object is closed from shared memory writter process\n");
    	if (close(shm_fd) == -1) {
        	perror("close");
        	exit(EXIT_FAILURE);
    	}
	sleep(10);
    return 0;
}


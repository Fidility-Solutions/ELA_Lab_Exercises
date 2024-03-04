/*
 * File Name: 	shared_memory_writer.c
 *
 * Description: This program demonstrates writing data to a POSIX shared memory object.
 *
 * Author: 	Fidility Solutions
 *
 * Usage: 	./shared_memory_writer
 *
 * Reference: 	The Linux Interface Programming book.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SHARED_MEM_SIZE 1024
#define SHARED_MEM_NAME "my_shared_memory"
/*
 * Function: main
 *
 * Description: Entry point of the program. Opens an existing shared memory object,
 *              maps it into the address space, writes data to it, and then closes
 *              the shared memory object.
 *
 * Parameters: None
 *
 * Returns: int - exit status
 *
 */
int main() {
	printf("\n\nYou have entered into Shared Memory writter Process!\n");
	printf("Welcome to POSIX Shared Memory writter Program\n");
	printf("The Writter Process id(PID):%d\n",getpid());
    	/* Open existing shared memory object */
	printf("Trying to open shared memory region...!\n");
    	int8_t s8FileDescriptor = shm_open(SHARED_MEM_NAME, O_RDWR, 0666);
    	if (s8FileDescriptor == -1) {
        	perror("shm_open error");
        	exit(EXIT_FAILURE);
    	}

    	/* Map the shared memory object into the address space */
	printf("Mapping the shared memory object into the address space...\n"); 
    	int8_t *ps8SharedMemory = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (ps8SharedMemory == MAP_FAILED) {
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}
	
	/* write data to shared memory object */
    	printf("Writing data to shared memory object...\n");
    	sprintf(ps8SharedMemory, "Hello from Shared Memory Writter Process");
    	printf("The Shared Memory Writter Process wrote: %s\n", ps8SharedMemory);

    	/* Unmap shared memory in the child process */
	printf("\nUnmapping the shared memory object...\n");
    	if (munmap(ps8SharedMemory, SHARED_MEM_SIZE) == -1) {
        	perror("munmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Close shared memory object */
	printf("The shared memory object is closed from shared memory writter process\n");
    	if (close(s8FileDescriptor) == -1) {
        	perror("close error");
        	exit(EXIT_FAILURE);
    	}
	sleep(10);
    return 0;
}


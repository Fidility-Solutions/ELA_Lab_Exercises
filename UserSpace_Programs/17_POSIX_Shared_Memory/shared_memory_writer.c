/*********************************************************************************************
 * File Name: 	shared_memory_writer.c
 *
 * Description: This program  creates/opens the shared memory object and write data to a 
 * 		POSIX shared memory object. It maps the shared memory object into memory and 
 * 		write data to shared memory object. Then unmap and close the shared memory object.
 *
 * Author: 	Fidility Solutions
 *
 * Usage: 	./shared_memory_writer
 *
 * Date: 	04/03/2024
 *
 * Reference: 	The Linux Interface Programming book.
 **********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SHARED_MEM_SIZE 1024
#define SHARED_MEM_NAME "POSIX_Shm_Rgn"

/* Error notification and exit program */
void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}
/* Error notification along with program name and exit the program */
void usageErr(const char *programName, const char *message) {
    fprintf(stderr, "Usage: %s %s\n", programName, message);
    exit(EXIT_FAILURE);
}

/*
 * Function: main
 *
 * Description: Entry point of the program. Opens an existing shared memory object,
 *              maps it into the address space, writes data to it and then closes
 *              the shared memory object.
 *
 * Parameters: 	None
 *
 * Returns: 	int - exit status
 *
 */
int main() {
	printf("\nThis is Shared Memory writer Process!\n");
	printf("Welcome to POSIX Shared Memory writer Program\n");
	printf("The Writer Process id(PID):%d\n",getpid());
    	/* Open existing shared memory object */
	printf("Opening the shared memory region...!\n");
    	int8_t s8FileDescriptor = shm_open(SHARED_MEM_NAME, O_CREAT|O_RDWR, 0666);
    	if (s8FileDescriptor == -1)
		errExit("shared memory open error");

    	/* Map the shared memory object into the address space */
	printf("Mapping the shared memory object into the address space...\n"); 
    	int8_t *ps8SharedMemory = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (ps8SharedMemory == MAP_FAILED) 
		errExit("shared memory mapping fail");	

	/* write data to shared memory object */
    	sprintf(ps8SharedMemory, "Hello from Shared Memory Writer Process");
    	printf("The Shared Memory Writer Process writing:\" %s\"\n", ps8SharedMemory);

    	/* Unmap shared memory in the child process */
	printf("\nUnmapping the shared memory object...\n");
    	if (munmap(ps8SharedMemory, SHARED_MEM_SIZE) == -1) 
        	errExit("unmapping error");

    	/* Close shared memory object */
	printf("The shared memory object is closed from shared memory writer process\n");
    	if (close(s8FileDescriptor) == -1)
	       errExit("shared memory region close error");	

	sleep(10);
    return 0;
}


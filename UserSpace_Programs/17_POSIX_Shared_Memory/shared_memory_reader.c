/*****************************************************************************************
 * File Name: 	shared_memory_reader.c
 *
 * Description: This program demonstrates reading data from a POSIX shared memory object.
 *
 * Author: 	Fidillity Solutions
 *
 * Usage: 	./shared_memory_reader
 *
 * Reference: 	The Linux Interface Programming book.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SHARED_MEM_SIZE 1024
#define SHARED_MEM_NAME "/my_shared_memory"

/*
 * Function: 	main
 *
 * Description: Entry point of the program. Creates a shared memory object, sets its size,
 *              maps it into the address space, creates a child process to replace itself
 *              with the writer process, reads data from the shared memory object, and then
 *              closes and unlinks the shared memory object.
 *
 * Parameters: None
 *
 * Returns: 	int - exit status
 */
int  main() {
	printf("Welcome to POSIX Shared Memory Reader Process Program \n");

    	/* Create shared memory object */
	printf("\nShared Memory is created in shared memory reader process \n");
    	int8_t s8FileDescriptor = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    	if (s8FileDescriptor == -1) {
        	perror("shm_open error");
        	exit(EXIT_FAILURE);
    	}

    	/* Set the size of the shared memory object */
	printf("Setting the Size to Shared Memory Object Using ftruncate () system call...\n");
    	if (ftruncate(s8FileDescriptor, SHARED_MEM_SIZE) == -1) {
        	perror("ftruncate error");
        	exit(EXIT_FAILURE);
    	}

    	/* Map the shared memory object into the address space of the Reader process */
	printf("Mapping the shared memory object into the address space ..\n");
    	char *parent_shared_memory = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (parent_shared_memory == MAP_FAILED) {
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Create a child process */
	printf("\nThe child process created from reader process it will replaced with writter process\n");
    	pid_t pid = fork();

    	if (pid < 0) {
        	perror("fork error");
        	exit(EXIT_FAILURE);
    	}
	/*child process */
    	else if (pid == 0) {
		printf("The Child process id(PID):%d\n",getpid());
		printf("The child process from shared memory reader process is replaced with shared memory writter process using execl system call\n");
        	execl("./shared_memory_writter", "shared_memory_writter", (char *)NULL);
        	perror("execl error");
        	exit(EXIT_FAILURE);
    	} 
	/* Parent Process */
	else {
		wait(NULL);
		printf("The Writter Process is Terminated!\n");
		printf("The Reader process id(PID):%d\n",getpid());
        	printf("\nReader process reading from shared memory...\n");
        	printf("Reader process read: %s \n", parent_shared_memory);
    	}
	printf("\nUnmapping Shared Memory in Shared Memory Reader Process\n");
    	/* Unmap shared memory in the parent process */

    	if (munmap(parent_shared_memory, SHARED_MEM_SIZE) == -1) {
        	perror("munmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Close and unlink shared memory object */
	printf("Closing Shared Memory Object \n");
    	if (close(s8FileDescriptor) == -1) {
        	perror("close error");
        	exit(EXIT_FAILURE);
    	}
	printf("Deleting Shared Memory Object...\n");
    	if (shm_unlink(SHARED_MEM_NAME) == -1) {
        	perror("shm_unlink error");
        	exit(EXIT_FAILURE);
    	}
	printf("Exited Shared Memory Reader Process\n");
	printf("\nThank you\n");

    	return 0;
}


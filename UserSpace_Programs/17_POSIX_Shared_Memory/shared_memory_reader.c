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
#define SHARED_MEM_NAME "/POSIX_Shm_Rgn"

void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void usageErr(const char *programName, const char *message) {
    fprintf(stderr, "Usage: %s %s\n", programName, message);
    exit(EXIT_FAILURE);
}




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
int  main(void){
	printf("Welcome to POSIX Shared Memory Reader Process Program \n");
	struct stat StrSb;
    	/* Create shared memory object */
	printf("\nShared Memory is created in shared memory reader process \n");
    	int8_t s8FileDescriptor = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    	if (s8FileDescriptor == -1) 
        	errExit("shm_open error");

	/* Set the size of the shared memory object */
        printf("Setting the Size to Shared Memory Object Using ftruncate () system call...\n");
        if (ftruncate(s8FileDescriptor, SHARED_MEM_SIZE) == -1)
                errExit("ftruncate error");
	if (fstat(s8FileDescriptor, &StrSb) == -1)
		errExit("fstat error");
	printf("The file Size is:%ld\n",StrSb.st_size);
	printf("The file permission:%d\n",StrSb.st_mode&0777);

    	/* Map the shared memory object into the address space of the Reader process */
	printf("Mapping the shared memory object into the address space ..\n");
    	char *ps8addr = mmap(NULL,StrSb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (ps8addr == MAP_FAILED) 
        	errExit("mmap error");

    	/* Create a child process */
	printf("\nThe child process created from reader process it will replaced with writter process\n");
    	pid_t pid = fork();

    	if (pid < 0)
        	errExit("fork error");

	/*child process */
    	else if (pid == 0) {
		printf("The Child process id(PID):%d\n",getpid());
		printf("The child process from shared memory reader process is replaced with shared memory writter process using execl system call\n");
        	execl("./shared_memory_writter", "shared_memory_writter", (char *)NULL);
        	errExit("execl error");
    	} 
	/* Parent Process */
	else {
		wait(NULL);
		printf("The Writter Process is Terminated!\n");
		printf("The Reader process id(PID):%d\n",getpid());
        	printf("\nReader process reading from shared memory...\n");
        	printf("Reader process read: %s \n", ps8addr);
    	}
	printf("\nUnmapping Shared Memory in Shared Memory Reader Process\n");
    	/* Unmap shared memory in the parent process */

    	if (munmap(ps8addr,StrSb.st_size) == -1)
	       errExit("unmapping failed");	
    	

    	/* Close and unlink shared memory object */
	printf("Closing Shared Memory Object \n");
    	if (close(s8FileDescriptor) == -1)
        	errExit("close error");

	printf("Deleting Shared Memory Object...\n");
    	if (shm_unlink(SHARED_MEM_NAME) == -1) 
        	errExit("shm_unlink error");
    	
	printf("Exited Shared Memory Reader Process\n");
	printf("\nThank you\n");

    	return 0;
}


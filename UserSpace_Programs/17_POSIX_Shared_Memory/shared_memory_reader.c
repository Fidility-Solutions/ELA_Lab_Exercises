/*****************************************************************************************
 * File Name: 	shared_memory_reader.c
 *
 * Description: This program demonstrates creation/opening of shared memory and mapping into memory address.
 * 		Create the child process and from child process it replaces with writer programs.Then
 * 		it will read the data from a POSIX shared memory object which is written by writer program.
 *
 * Author: 	Fidillity Solutions
 *
 * Usage: 	./shared_memory_reader
 *
 * Date:	04/03/2024
 *
 * Reference: 	"The Linux Programming Interface" book.
 *
 ****************************************************************************************/

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
/*
 * function:	errExit
 *
 * Description:	This function will notify the error which is cased by the program and exit from the program 
 *
 * parameter:	constant char types pointer variable.
 *
 * Return:	None
 *
 */
void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}


/*
 * Function: 	main
 *
 * Description: Entry point of the program. Creates a shared memory object, sets its size,
 *              maps it into the address space, creates a child process to replace itself
 *              with the writer process, reads data from the shared memory object. Then
 *              closes and unlinks the shared memory object.
 *
 * Parameters: None
 *
 * Returns: 	int - exit status
 *
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
        printf("Setting the size of a shared memory object is accomplished by using the ftruncate() system call with the file descriptor.\n");
        if (ftruncate(s8FileDescriptor, SHARED_MEM_SIZE) == -1)
                errExit("ftruncate error");
	if (fstat(s8FileDescriptor, &StrSb) == -1)
		errExit("fstat error");
	printf("The file Size is:%ld\n",StrSb.st_size);
	printf("The file permission to read and write :%d\n",StrSb.st_mode&0777);

    	/* Map the shared memory object into the address space of the Reader process */
	printf("Mapping the shared memory object into the address space ..\n");
    	char *ps8addr = mmap(NULL,StrSb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (ps8addr == MAP_FAILED) 
        	errExit("mmap error");

    	/* Create a child process */
	printf("\nThe child process created from reader process is replaced with writer process\n");
    	pid_t pid = fork();

    	if (pid < 0)
        	errExit("fork error");

	/*child process */
    	else if (pid == 0) {
		printf("The Child process id(PID):%d\n",getpid());
		printf("The child process from shared memory reader process is replaced with shared memory writer process using execl system call\n");
        	execl("./shared_memory_writter", "shared_memory_writter", (char *)NULL);
        	errExit("execl error");
    	} 
	/* Parent Process */
	else {
		wait(NULL);
		printf("\nThe Writer Process is Terminated!\n");
		printf("The Reader process id(PID):%d\n",getpid());
        	printf("Reader process reading from shared memory \"%s\" \n", ps8addr);
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


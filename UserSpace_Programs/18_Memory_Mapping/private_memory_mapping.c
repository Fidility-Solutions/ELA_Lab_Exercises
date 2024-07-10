/*
 * File:        private_memory_mapping.c
 *
 * Description: This program demonstrates the usage of private shared memory mapping between parent and child 
 * 		processes.It creates a file, maps it into memory, writes data to the mapped memory in the parent 
 * 		process,and try to reads the data from the mapped memory in the child process and print error message. 
 * 		Both processes then unmap the memory and close the file. This program illustrates inter-process 
 * 		communication using private memory_mapping.
 *
 * Usage:       ./private_memory_mapping
 *
 * Author:      Fidility Solutions
 *
 * Date:        04/03/2024
 *
 * Reference:   The Linux Programming Interface book.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdint.h>

#define FILE_PATH "Private_Mapped_file.txt"
#define FILE_SIZE 100
int8_t *ps8addr;
int8_t s8FileDescriptor;

/* Function	: ParentProcess
 *
 * Description	: Creates a private shared memory mapping of a file, writes data to the mapped memory,
 *               waits for the child process to finish, and then unmaps the memory.
 *
 * Return	: None
 */
void ParentProcess(void) {
	/* Wait for the child process to finish */
	wait(NULL);

	printf("\nEntered into Parent Process\n");
	sleep(2);
	/* Read from the mapped memory */
        printf("Parent read from mapped memory: %s\n", ps8addr);
    	/* Unmap the memory */
    	if (munmap(ps8addr, FILE_SIZE) == -1) {
        	perror("munmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Close the file */
    	if (close(s8FileDescriptor) == -1) {
        	perror("close error");
        	exit(EXIT_FAILURE);
    	}
	printf("Exiting from parent process...\n");
	sleep(2);
}

/* Function	: 	ChildProcess
 *
 * Description	: Creates a private shared memory mapping of a file, tries to read data from the mapped memory,
 *              if it fails to read print error message.
 *
 * Return	: None
 */
void ChildProcess(void) {
	printf("\nEntered into Child Process\n");
        printf("Child read data from mapped memory: %s\n",ps8addr);
        
        /* Write to the mapped memory from child */
	sleep(3);
        const char *child = "Hello from child process!";
        strcpy(ps8addr, child);
	/* Read back what child wrote */
	sleep(2);
        printf("Child wrote and read from mapped memory: %s\n", ps8addr);

    	/* Unmap the memory */
    	if (munmap(ps8addr, FILE_SIZE) == -1) {
        	perror("munmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Close the file */
    	if (close(s8FileDescriptor) == -1) {
        	perror("close error");
        	exit(EXIT_FAILURE);
    	}
	printf("Exiting from child process...\n");
	sleep(2);
    	exit(EXIT_SUCCESS);
}

/* Function	: main
 *
 * Description	: Creates a child process, where the child process try to reads from a shared memory mapping
 *            	  created by the parent process and printf the status.
 *
 * Return	: 0 on successful execution, non-zero value on failure.
 */
int main() {
	printf("Welcome to Parent & Child private memory mapping process\n");

     	struct stat strinfo;

     	/* Open the file */
     	printf("Creating a file for mapping... \n");
     	s8FileDescriptor = open(FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
     	if (s8FileDescriptor == -1) {
     	        perror("open error");
     	        exit(EXIT_FAILURE);
     	}

     	/* Truncate the file to the desired size */
     	printf("Allocating size of the file to be mapped...\n");
     	if (ftruncate(s8FileDescriptor, FILE_SIZE) == -1) {
     	        perror("ftruncate error");
     	        exit(EXIT_FAILURE);
     	}
     	if (fstat(s8FileDescriptor, &strinfo) == -1) {
     	          perror("fstat");
     	          exit(EXIT_FAILURE);
     	  }
     	  /* printing file inforamtion */
     	  printf("File Size: %ld bytes\n", strinfo.st_size);
     	  printf("File Permissions: %o\n", strinfo.st_mode & 0777);


     	/* Map the file into memory */
     	printf("Mapping the file into memory...\n");
     	ps8addr = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, s8FileDescriptor, 0);
     	if (ps8addr == MAP_FAILED) {
     	        perror("mmap error");
     	        exit(EXIT_FAILURE);
     	 }
	/* Write data to the mapped memory */
	
	strcpy(ps8addr, "\"Welcome to private memory mapping from IPC\"");
	printf("Data written to mapped memory: %s\n",ps8addr);
	printf("\n");
   	/* Create child process */
   	pid_t pid = fork();

   	if (pid == -1) {
       		perror("fork");
       		exit(EXIT_FAILURE);
   	} 
     	else if (pid == 0) {
       		/* Child process */
       		ChildProcess();
   	} 
     	else {
       		/* Parent process */
		wait(NULL);
       		ParentProcess();
   	}
	unlink(FILE_PATH);
	printf("\nExiting from main program...\n");
    	return 0;
}


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

/* Function	: ParentProcess
 *
 * Description	: Creates a private shared memory mapping of a file, writes data to the mapped memory,
 *               waits for the child process to finish, and then unmaps the memory.
 *
 * Return	: None
 */
void ParentProcess(void) {
	printf("Entered into Parent Process\n");
    	int8_t s8FileDescriptor;
    	int8_t *ps8addr;
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
    	strcpy(ps8addr, "\"Hello welcome to private memory mapping from IPC\"");
	printf("Data written by the Parent Process:%s\n",ps8addr);
    	/* Wait for the child process to finish */
    	wait(NULL);

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
}

/* Function	: 	ChildProcess
 *
 * Description	: Creates a private shared memory mapping of a file, tries to read data from the mapped memory,
 *              if it fails to read print error message.
 *
 * Return	: None
 */
void ChildProcess(void) {
	sleep(2);
	printf("\nEntered into child process \n");
    	int s8FileDescriptor;
    	int8_t *ps8addr;

    	/* Open the file */
	printf("Opening mapped file ...\n");
    	s8FileDescriptor = open(FILE_PATH, O_RDWR);
    	if (s8FileDescriptor == -1) {
        	perror("open error");
        	exit(EXIT_FAILURE);
    	}

    	/* Map the file into memory */
    	ps8addr = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE, s8FileDescriptor, 0);
    	if (ps8addr == MAP_FAILED) {
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Print data from the mapped memory */
	printf("Trying to read data from mapped file...\n");
	sleep(3);
    	if(ps8addr == NULL)
	{
		printf("Data from shared memory:%s\n",ps8addr);
	}
	else{
		printf("Error:\"The content from the mapped file is not accessed by child process because it is private memory mapping\"\n");
		printf("Note:\'All processes that have the memory mapped can read from and write to their own private view of the mapped file.\n In this program, Only the Parent have access to read from and write to mapped file\'\n");
	}

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
	printf("\nExiting from child process...\n");
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
        	ParentProcess();
    	}
	unlink(FILE_PATH);
	printf("Exiting from main program...\n");
    	return 0;
}


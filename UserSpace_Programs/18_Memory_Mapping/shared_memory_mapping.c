/*
 * File:        shared_memory_mapping.c
 *
 * Description: This program demonstrates the usage of shared memory mapping between parent and child processes.
 *              It creates a file, maps it into memory, writes data to the mapped memory in the parent process,
 *              and reads the data from the mapped memory in the child process. Both processes then unmap the
 *              memory and close the file. This program illustrates inter-process communication using 
 *              shared memory_mapping.
 *
 * Usage:       ./shared_memory_mapping
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

#define FILE_PATH "Shared_Mapped_file.txt"
#define FILE_SIZE 100
/*
 * function:    errExit
 *
 * Description: This function will notify the error which is cased by the program and exit from the program 
 *
 * parameter:   constant char types pointer variable.
 *
 * Return:      None
 *
 **/
void errExit(const char *message) {
    errExit(message);
    exit(EXIT_FAILURE);
}

/* Function: 	ParentProcess
 * Description: Creates a shared memory mapping of a file, writes data to the mapped memory,
 *              waits for the child process to finish, and then unmaps the memory.
 * Return: None
 */
void ParentProcess(void) {
	printf("Entered into Parent Process\n");
    	int8_t s8FileDescriptor;
    	int8_t *ps8addr;
	struct stat strinfo;

    	/* Open the file */
	printf("Creating mapped file... \n");
    	s8FileDescriptor = open(FILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    	if (s8FileDescriptor == -1)
	       errExit("file open fail");	
    	
    	/* Truncate the file to the desired size */
	printf("Allocating size to mapped file ...\n");
    	if (ftruncate(s8FileDescriptor, FILE_SIZE) == -1) 
        	errExit("ftruncate error");
    	
	if (fstat(s8FileDescriptor, &strinfo) == -1) 
                errExit("fstat error");
        
        /* printing file inforamtion */
        printf("File Size: %ld bytes\n", strinfo.st_size);
        printf("File Permissions: %o\n", strinfo.st_mode & 0777);


    	/* Map the file into memory */
	printf("Mapping mapped file into memory...\n");
    	ps8addr = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (ps8addr == MAP_FAILED) 
        	errExit("mmap error");

    	/* Write data to the mapped memory */
    	strcpy(ps8addr, "Hello welcome to memory mapping in IPC");
	printf("Written data to mapped file\n");


    	/* Wait for the child process to finish */
    	wait(NULL);

    	/* Unmap the memory */
    	if (munmap(ps8addr, FILE_SIZE) == -1) 
        	errExit("munmap error");

    	/* Close the file */
    	if (close(s8FileDescriptor) == -1) 
        	errExit("close error");
    	wait(NULL);
	printf("Exiting from parent process...\n");
}

/* Function: 	ChildProcess
 *
 * Description: Creates a shared memory mapping of a file, reads data from the mapped memory,
 *              prints it, and then unmaps the memory.
 *
 * Return: None
 */
void ChildProcess(void){
	sleep(2);
	printf("\nEntered into child process \n");
    	int s8FileDescriptor;
    	int8_t *ps8addr;

    	/* Open the file */
	printf("Opening mapped file ...\n");
    	s8FileDescriptor = open(FILE_PATH, O_RDWR);
    	if (s8FileDescriptor == -1) 
		errExit("file open error");

    	/* Map the file into memory */
    	ps8addr = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s8FileDescriptor, 0);
    	if (ps8addr == MAP_FAILED) 
        	errExit("mmap error");

    	/* Print data from the mapped memory */
    	printf("Data from parent process: \"%s\"\n", ps8addr);

    	/* Unmap the memory */
    	if (munmap(ps8addr, FILE_SIZE) == -1) 
        	errExit("munmap error");

    /* Close the file */
    	if (close(s8FileDescriptor) == -1) 
        	errExit("close error");
    	
	printf("\nExiting from child process...\n");
    	exit(EXIT_SUCCESS);
}

/* Function: 	main
 *
 * Description: Creates a child process, where the child process reads from a shared memory mapping
 *              created by the parent process.
 *
 * Return: 0 on successful execution, non-zero value on failure.
 *
 */
int main() {
	printf("Welcome to Parent & Child shared memory mapping process\n");
    	/* Create child process */
    	pid_t pid = fork();

    	if (pid == -1) 
        	errExit("fork");
	else if (pid == 0) {
        	/* Child process */
        	ChildProcess();
    	} 
	else {
        	/* Parent process */
        	ParentProcess();
    	}
	printf("Exiting from main program...\n");
    	return 0;
}


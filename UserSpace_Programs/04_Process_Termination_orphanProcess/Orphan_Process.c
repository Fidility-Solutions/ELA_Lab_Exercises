/*******************************************************************************
 * File:        Orphan_Process.c
 *
 * Description: This file contains a C program demonstrating the concept of an orphan process. An orphan process 
 *              occurs when a child process outlives its parent process. In this program, a parent process forks 
 *              a child process. The parent process then waits for a short time before exiting, while the child 
 *              process sleeps for a longer time to simulate longer execution. As a result, the child process 
 *              becomes an orphan when the parent process exits before it does. The init process adopts the orphan 
 *              process until it completes its execution.
 *              
 * Author:      Fidility Solutions.
 *
 * Date:        23/02/2024.
 *
 * Reference    The "Linux Programming Interface" book.

* *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<sys/wait.h>

/*
 * Function	: main()
 *
 * Description	: Entry point of the program. Demonstrates the concept of an orphan process.
 *
 * Parameters	: None
 *  
 * Returns	: 0 on successful execution, 1 on failure.
 *
 */

int main(void)
{
	printf("Program to demonstrate orphan process\n");
	/* Create a Child Process using fork() system call  */
	pid_t pid = fork();
	if (pid < 0)
	{
		perror("fork failed");
        	exit(EXIT_FAILURE);
	}
	/* This is Child Process because the fork() returns zero */
	else if (pid == 0)
	{
		printf("Child process ID (PID):%d and its parent process ID (PPID):%d\n",getpid(),getppid());
        	printf("\nChild Process Entering into sleep\n");
		/* Child process sleeps for a longer time to simulate longer execution time */
        	sleep(30);
		printf("\nAfter sleep Child process (PID): %d and parent Process (PID):%d\n", getpid(),getppid());
		printf("child process exited\n");
		exit(1);
	}
	 /*  Parent of Child Process */
	else
	{
        	/* The parent process terminates immediately after creating the child */
		printf("Parent process ID (PID): %d \n", getpid());
		sleep(20);
		printf("Parent Process is exited\n");

	}
    return 0;
}



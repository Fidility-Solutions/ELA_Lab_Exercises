/********************************************************************************
 * File		: Zombie_Process.c
 *
 * Description	: This file contains a C program demonstrating the concept of a zombie process. A zombie process 
 *              occurs when a child process completes execution, but its exit status has not yet been retrieved 
 *              by its parent process. 
 *              In this program, a parent process forks a child process. The child process immediately exits, 
 *              while the parent process waits for a short time before exiting. 
 *              As a result, the child process becomes a zombie until the parent process retrieves its exit 
 *              status using the wait() system call.
 *
 * Author	: Fidility Solutions.
 *
 * Date	  	: 23/02/2024.
 *
 * Reference   	: The "Linux Programming Interface" book.
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*
 * Function	: main()
 *
 * Description	: Entry point of the program. Demonstrates the concept of an zombie process.
 *
 * Parameters	: None
 *  
 * Returns	: 0 on successful execution, 1 on failure.
 *
 */

int main(void)
{
	printf("This program demonistrates how process become zombie\n");
	/* Create a Child Process using fork() system call  */
	pid_t child = fork();
    	if (child < 0)
    	{
        	perror("fork failed");
        	exit(EXIT_FAILURE);
	}
	/* This is Child Process because the fork() returns zero */
	else if(child == 0)
	{
		printf("\nChild process created\n");
        	printf("Child process ID(PID): %d created with parent process ID(PID): %d\n", getpid(), getppid());
		/* Child process exits immediately after printing */       
		sleep(5);
		printf("The Child Process completed its execution\n");
		exit(1);
	}
       /*  Parent of Child Process */	
	else 
	{
        	printf("Parent process ID(PID): %d is entering into sleep\n", getpid());
		/* Parent process waits for a few seconds before terminating */
		sleep(10);
		printf("\nAfter sleep the Parent process exited\n");
		return 0;
   	}
	return 0;
}


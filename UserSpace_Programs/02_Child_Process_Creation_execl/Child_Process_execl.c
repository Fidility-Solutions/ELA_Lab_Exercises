/*******************************************************************************
 * File		: Child_Process_Creation.c
 *
 * Description	: This program will create the child process using fork() system call 
 * 		  and it replace the child process image with new process using execve() system call.
 *              
 * Author	: Fidility Solutions.
 *
 * Date		: 23/02/2024.
 *
 * Reference    : The "Linux Programming Interface" book.

* *******************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>


/*
 * Function	: main()
 *
 * Description	: Entry point of the program. This function demonstrates the usage of the fork() system call
 *              to create a child process.The parent process waits for the child to finish executing.
 *              If the fork operation succeeds, two separate processes are created: the parent process
 *              and the child process. The child process replaces its own process image with a new program
 *              image specified by the execl() function, effectively executing the "/bin/ls -l" command.
 *              Meanwhile, the parent process waits for the child to complete its execution before printing
 *              a message indicating that the child process has finished.
 *
 * Parameters	: None
 *  
 * Returns	: 0-on success
 *
 */


int main(void)
{ 
	printf("This porgram demonstrates the use of execl () system call\n");
	/* Create a Child Process using fork() system call  */
	pid_t child = fork();
	int status;
	if(child == -1)
	{
		perror("fork failure");
		exit(EXIT_FAILURE);
	}
	/* This is Child Process because the fork() returns zero */
	else if(child == 0)
	{
		printf("Child process created with id (PID):%d and its parent Process id(PID)=%d\n",getpid(),getppid());
		/* Execute the program with execl */
		/* Replacing child process with new process using execl() system call */
		printf("Child process is replaced with new process(ls command) by using execl() system call \n");
		if(execl("/bin/ls","ls","-l",NULL)== -1)
		{
			/* execl function returns only if an error occurs */
			perror("execl failure");
			exit(EXIT_FAILURE);
		}
	}
	/*  Parent of Child Process */
	else
	{
		/* wait for Child Process to terminate */
		wait(&status);
		if (WIFEXITED(status)) 
		{
			int exit_status = WEXITSTATUS(status);
			printf("\nChild process exited normally with status %d\n", exit_status);
		} 
		else if (WIFSIGNALED(status)) 
		{
			int signal_number = WTERMSIG(status);
			printf("Child process terminated due to signal %d\n", signal_number);
		} 
		else 
		{
			printf("Unknown child process termination status\n");
		}
	}
	printf("Exiting from parent process\n");
	sleep(15);
	return 0;
}


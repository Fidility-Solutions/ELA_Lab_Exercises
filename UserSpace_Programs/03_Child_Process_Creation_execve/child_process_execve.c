/*******************************************************************************
 * File  	: Child_Process.c
 *
 * Description	: This program will create the child process using fork() system call 
 * 		and it replace the child process image with new process using execve() system call.
 *
 * Author	: Fidility Solutions.
 *
 * Date		: 23/02/2024.
 *
 * Reference 	: The "Linux Programming Interface" book.

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
 *              image specified by the execve() function, effectively executing the "/bin/echo Hello, World!" command.
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
        /* Parent Process */
	printf("This program demonstrates the use of execve() function\n");
	printf("Parent Process ID(PID):%d\n",getpid());
	/* Create a Child Process using fork() system call  */
	char *path = "./print_data";
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
		printf("\nChild Process created with PID= %d and its parent PID = %d\n",getpid(),getppid());
		/* Command-line arguments to be passed */
		char *const argv[] = {"print_data", "Hello,", "this", "is", "a", "test.", NULL};
		/* Environment variables to be passed */
		char *const envp[] = {"MY_VAR1=Hello", "MY_VAR2=World", NULL};
		/* Execute the program with execve */
		printf("Child process is replaced with \"print_data\", it display argv and envp passsed\n");
		
		/* Replacing child process with new process using execve() system call */
		if(execve(path,argv,envp)== -1)
		{
			/* execve function returns only if an error occurs */
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


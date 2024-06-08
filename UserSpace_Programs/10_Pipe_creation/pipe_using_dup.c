/******************************************************************************
 * File:        pipe_using_dup.c
 *
 * Description: This program demonstrates inter-process communication using pipes. It sets up a parent-child 
 * 		relationship where the parent process lists the contents of a directory using the 'ls' command,
 *              and the child process counts the number of lines in the output using the 'wc -l' command. 
 *              Both parent and child processes are replaced with new processes when executing these commands 
 *              using execlp(). The program utilizes pipes to establish communication between the parent and 
 *              child processes.
 *
 * Usage:       ./pipe_using_dup
 * 
 * Author:      Fidility Solutions.
 *  
 * Date:        27/02/2024.
 *
 * Reference    The "Linux Programming Interface" book
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

#define MSG_SIZE 256
/*
 * Function: main
 * 
 * Description: Entry point of the program. Initializes inter-process communication using pipes, 
 * 		forks a child process, and configures the child process to count the number of lines
 *              received from the parent process.
 *
 * Parameters:   None
 *
 * Returns:     0 upon successful execution of the program.
 */

int main(void)
{
	printf("Welcome to Inter-Process Communication using Pipe\n");

	/* file descriptors */
    	int32_t as8PipeFd[2];
    	pid_t pid;

    	/* Create pipe */
    	if(pipe(as8PipeFd) == -1){
        	perror("pipe error");
        	exit(EXIT_FAILURE);
    	}
	printf("Pipe is created successfully\n");
    	/* Fork a child process */
    	pid = fork();
    	if(pid == -1){
        	perror("fork error");
        	exit(EXIT_FAILURE);
    	}

    	/* Child process */
	else if(pid == 0){

        	/* Close unused write end */
        	close(as8PipeFd[1]);
		printf("\nChild process is created with process id(PID):%d\n",getpid());
		
		/* Duplicate stdin on read end of pipe; close duplicated descriptor */
        	if(dup2(as8PipeFd[0], STDIN_FILENO) == -1){
            		perror("dup2 error");
            		exit(EXIT_FAILURE);
        	}	

		/* close the unused file */
        	close(as8PipeFd[0]);

        	/* execute the consumer filter(eg. wc commond) */
		printf("Child process is executing the 'wc -l' command using execl()\n");
		printf("Child process takes input 'ls' passed from parent process and then it performs operation(ls|wc -l):\n");
		
	       	execlp("/bin/wc","wc","-l",NULL);
		
		/* if execlp fails print error */
		/*This line shouldn't be reached if execlp successful */
            	perror("ececlp error");
            	exit(EXIT_FAILURE);
        }

    /* Parent process */
    	else{
		printf("\nParent process id(PID):%d\n",getpid());
		printf("Parent process is replaced with ls command using execlp ()\n");

        	/*  Close unused read end parent will only write */
        	close(as8PipeFd[0]);
		/* redirect stdout to write to the write end of pipe */
		dup2(as8PipeFd[1],STDOUT_FILENO);
		/*close the unused descriptor */

		/* execute the producer filter(eg. ls command) */
//		chdir("/home/fidility/");
		execlp("ls","ls",NULL);
		/* if execlp fails print error */
            	perror("write error");
            	exit(EXIT_FAILURE);
        }

    return 0;
}


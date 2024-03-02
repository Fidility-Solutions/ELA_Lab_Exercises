/******************************************************************************
 * File:        pipe_creation.c
 *
 * Description: This program demonstrates inter-process communication using
 *              pipes. It sets up a parent-child relationship where the parent
 *              process lists the contents of a directory using the 'ls' command,
 *              and the child process counts the number of lines in the output
 *              using the 'wc -l' command. Both parent and child processes are
 *              replaced with new processes when executing these commands using
 *              execlp(). The program utilizes pipes to establish communication
 *              between the parent and child processes.
 *
 * Usage:       ./pipe_creation
 * 
 * Author:      Fidility Solutions.
 *  
 * Date:        29/02/2024.
 *
 * Reference    The Linux Programming Interface book
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG_SIZE 256
/*
 * Function: main
 * 
 * Description: Entry point of the program. Initializes inter-process
 *              communication using pipes, forks a child process, and
 *              configures the child process to count the number of lines
 *              received from the parent process.
 *
 * Arguments:   None
 *This program demonstrates inter-process communication using
 *              pipes. It sets up a parent-child relationship where the parent
 *              process lists the contents of a directory using the 'ls' command,
 *              and the child process counts the number of lines in the output
 *              using the 'wc -l' command. Both parent and child processes are
 *              replaced with new processes when executing these commands using
 *              execlp(). The program utilizes pipes to establish communication
 *              between the parent and child processes.
 * Returns:     0 upon successful execution of the program.
 */

int main() {
	/* file descriptors */
    	int pipefd[2];
    	pid_t pid;

    /* Create pipe */
    	if (pipe(pipefd) == -1) {
        	perror("pipe");
        	exit(EXIT_FAILURE);
    	}

    	/* Fork a child process */
    	pid = fork();
    	if (pid == -1) {
        	perror("fork");
        	exit(EXIT_FAILURE);
    	}

    /* Child process */
	else if (pid == 0) {
        	/* Close unused write end */
        	close(pipefd[1]);
		printf("Child process is created and the child process id(PID):%d\n",getpid());
		
		/* Duplicate stdin on read end of pipe; close duplicated descriptor */
        	if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            		perror("dup2");
            		exit(EXIT_FAILURE);
        	}	

		/* close the unused file */
        	close(pipefd[0]);

        	/* execute the consumer filter(eg. wc commond) */
		printf("The child process is replacing with wc -l command using execl () sytem call \n");
	       printf("The child process take input passed from parent process is ls then it perform opeation(ls|wc -l)and print number of word count in current directory is:\n");
		execlp("/bin/wc","wc","-l",NULL);
		/* if execlp fails print error */
		/*This line shouldn't be reached if execlp successful */
            	perror("ececlp");
            	exit(EXIT_FAILURE);
        }

    /* Parent process */
    	else {
		printf("The parent process id(PID):%d\n",getpid());
		printf("The parent process is replacing with ls command using execlp () system call\nNow ls commad passing input to child process \n");

        	/*  Close unused read end parent will only write */
        	close(pipefd[0]);
		/* redirect stdout to write to the write end of pipe */
		dup2(pipefd[1],STDOUT_FILENO);
		/*close the unused descriptor */

		/* execute the producer filter(eg. ls command) */
//		chdir("/home/fidility/");
		execlp("ls","ls",NULL);
		/* if execlp fails print error */
            	perror("write");
            	exit(EXIT_FAILURE);
        }

    return 0;
}


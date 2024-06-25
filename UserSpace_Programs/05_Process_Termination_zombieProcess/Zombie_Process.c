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

#include <signal.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#define CMD_SIZE 200


/*
 * Function     : main()
 *
 * Description  : Entry point of the program. Demonstrates the concept of an zombie process.
 *
 * Parameters   : None
 *
 * Returns      : 0 on successful execution, 1 on failure.
 *
 */

int main(int argc, char *argv[])
{
	printf("This program demonistrates how process become zombie\n");
	char cmd[CMD_SIZE];
	pid_t childPid;
	setbuf(stdout, NULL);
	/* Disable buffering of stdout */
	printf("Parent PID=%ld\n", (long) getpid());
	switch (childPid = fork()) {
		case -1:
			perror("fork error");
			exit(EXIT_FAILURE);
		case 0:
		/* Child: immediately exits to become zombie */
			printf("Child (PID=%ld) exiting\n", (long) getpid());
			exit(EXIT_SUCCESS);
		default:	/* Parent */
			sleep(3);
			/* Give child a chance to start and exit */
			snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
			cmd[CMD_SIZE - 1] = '\0';
			/* Ensure string is null-terminated */
			printf("running ps command to check process entry in process table\n");
			system(cmd);
			/* View zombie child */
			/* Now send the "sure kill" signal to the zombie */
			if (kill(childPid, SIGKILL) == -1){
					perror("failed to kill child");
					exit(EXIT_FAILURE);
			}
			printf("sent kill signal to zombie process\n");
			sleep(3);
			/* Give child a chance to react to signal */
			printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);
			system(cmd);
			printf("Since the zombies can’t be killed by a signal, \nThe only way to remove them from the system is to kill their parent (or wait for it to exit), \nAt which time the zombies are adopted and waited on by init, and consequently removed from the system.\n");
			/* View zombie child again */
			exit(EXIT_SUCCESS);
	}
}

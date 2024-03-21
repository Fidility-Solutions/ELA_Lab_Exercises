#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/* Signal handler function for SIGUSR1 */
void sigusr1_handler(int signum) {
    	printf("Child Process: Received SIGUSR1 signal.\n");
}

int main(void){
    	pid_t pid;

    	/* Fork a child process */
    	pid = fork();
    	if(pid == -1){
        	perror("fork");
        	exit(EXIT_FAILURE);
    	} 
    	else if(pid == 0){
        	/* Child process*/
        	signal(SIGUSR1, sigusr1_handler); /* Register SIGUSR1 signal handler */
        	printf("Child Process: Waiting for SIGUSR1 signal...\n");
		/* Wait for signal */
        	pause(); 
        	printf("Child Process: Exiting.\n");
        	exit(EXIT_SUCCESS);
    	} 
	else{
        	/* Parent process */
        	printf("Parent Process: Sending SIGUSR1 signal to child...\n");
        	/* Sleep for 2 seconds before sending signal */
		sleep(2);
		/* Send SIGUSR1 signal to child */
        	kill(pid, SIGUSR1);

        /* Wait for child to terminate */
        wait(NULL);
        printf("Parent Process: Child terminated.\n");
	sleep(3);
    }

    return 0;
}


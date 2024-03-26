#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/* Signal handler function for SIGUSR1 */
void sigusr1_handler(int signum){
    	printf("Child Process: Received SIGUSR1 signal.\n");
}

int main(void){
	printf("This program demonistates the signal send from parent to child\n");
    	pid_t pid;

    	/* Fork a child process */
    	pid = fork();
    	if(pid == -1){
        	perror("fork");
        	exit(EXIT_FAILURE);
    	} 
    	else if(pid == 0){
        	/* Child process*/
		printf("\nThis is child process and my PID is:%d\n",getpid());

		/* Register SIGUSR1 signal handler */
		printf("Child Process: SIGUSR1 is registered to handle the signal\n");
        	signal(SIGUSR1, sigusr1_handler); 
        	printf("Child Process: Waiting for SIGUSR1 signal to terminate...\n");
		/* Wait for signal from parent*/
        	pause(); 
        	printf("Child Process: Exiting.\n");
        	exit(EXIT_SUCCESS);
    	} 
	else{
        	/* Parent process */
        	printf("Parent Process: Sending SIGUSR1 signal to child...\n");
        	/* Sleep for 2 seconds before sending signal */
		/* Press Enter to send signal */
		printf("Parent Process: Press 'Enter' to send signal to child\n");
		getchar();
		/* Send SIGUSR1 signal to child */
        	kill(pid, SIGUSR1);

        /* Wait for child to terminate */
        wait(NULL);
        printf("Parent Process: Child terminated.\n");
	sleep(3);
    }

    return 0;
}


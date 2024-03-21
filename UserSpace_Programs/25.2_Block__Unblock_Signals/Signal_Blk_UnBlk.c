#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>
void signal_handler(int sig){
	static int count=0;
	count++;
	if(sig == SIGINT){
		printf("Caught SIGINT(%d) for count value:%d\n",sig,count);
		return;
	}
	if(sig == SIGUSR1){
		printf("Recieved SIGUSR1(%d) for count value:%d\n",sig,count);
		return;
	}
	else
	{
		printf("Recieved SIGUSR2(%d) for count value:%d\n",sig,count);
		return;
	}
}

int main(void){
	printf("Welcome to Blocking & Unblocking the signals\n");
	/* Define a signal set for blocking signals */
    	sigset_t BlkMsk;
    	/* Initialize an empty signal set (no signals are initially blocked) */
	if (sigemptyset(&BlkMsk) == -1){
        	perror("sigemptyset");
        	exit(EXIT_FAILURE);
    	}

    	/* Add SIGINT to the signal set to block it*/
    	if(sigaddset(&BlkMsk, SIGINT) == -1){
        	perror("sigaddset");
        	exit(EXIT_FAILURE);
    	} 
	/* Block SIGINT using the signal set */
    	if(sigprocmask(SIG_BLOCK, &BlkMsk, NULL) == -1) {
       	 	perror("sigprocmask");
        	exit(EXIT_FAILURE);
    	}
	printf("Signals blocked\n");
	
    	/* Register signal handler for SIGINT */
    	if(signal(SIGINT, signal_handler) == SIG_ERR){
		perror("SIGINT fail");
		exit(EXIT_FAILURE);
	}
	/* Register signal handler for SIGUSR1 */
    	if(signal(SIGUSR1, signal_handler) == SIG_ERR){
		perror("SIGUSR1 fail");
		exit(EXIT_FAILURE);
	}
	/* Register signal handler for SIGUSR2 */
        if(signal(SIGUSR2, signal_handler) == SIG_ERR){
                perror("SIGUSR1 fail");
                exit(EXIT_FAILURE);
        }
	pid_t child=fork();
	if(child<0){
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	/* Child Process */
	if(child ==0){
		int value;
		sleep(2);
		printf("This is new child process\n");
		printf("Please Enter value (1 or 2) to handle custom signals:\n");
		for(;;){
			/* User input */
			if(scanf("%d",&value)==1){
				if(value == 1){
					kill(getppid(), SIGUSR1);
				}
				else if(value ==2){
					kill(getppid(), SIGUSR2);
				}
				else{
					printf("Please Enter valid input\n");
					exit(EXIT_FAILURE);
				}
			}
		}
	}
    	printf("My PID is %d\n", getpid());

     	/* Print a message indicating that the program is waiting for SIGINT& SIGQUIT signal */
    	printf("Waiting for SIGINT signal (Press \"Ctrl+C\" to send SIGINT)...\n");
    	printf("Press \"Ctrl+\\\" or \"Ctrl+z\" to quit...\n");

    	/* Unblock SIGINT */
    	if(sigprocmask(SIG_UNBLOCK, &BlkMsk, NULL) == -1){
      	 	perror("sigprocmask");
        	exit(EXIT_FAILURE);
    	}
    	printf("Signals Unblocked.\n");
	/* Keep the program running indefinitely, waiting for signals */
    	for(;;){
		/* Block until a signal is caught */
		pause();
	}

    	return 0;
}


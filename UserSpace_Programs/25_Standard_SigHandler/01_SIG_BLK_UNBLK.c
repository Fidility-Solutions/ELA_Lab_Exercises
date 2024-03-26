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
	exit(EXIT_SUCCESS);
}

int main(void){
	printf("Welcome to Blocking & Unblocking standard signals\n");
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
	printf("Signals blocked,so no operation will perform until unblock the signals\n");
	
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
	else if(child ==0){
		int value;
		sleep(2);
		printf("\nThis is child process and my PID is:%d\n",getpid());
		printf("Please Enter value (1 or 2) to handle custom signals(SIGUSR1 & SIGUSR2):\n");
		printf("For termination of child enter '6'\n");
		for(;;){
			/* User input */
			if(scanf("%d",&value)==1){
				if(value == 1){
					kill(getppid(), SIGUSR1);
				}
				else if(value ==2){
					kill(getppid(), SIGUSR2);
				}
				else if(value == 6){
					printf("Child process terminated, so custom signals won't work\n");
					exit(EXIT_SUCCESS);
				}
				else{
					printf("Please Enter valid input\n");
				//	exit(EXIT_FAILURE);
				}
			}
		}
	}
	else{
    		printf("\nThis Parent Process and My PID is: %d\n", getpid());
     		/* Print a message indicating that the program is waiting for SIGINT& SIGQUIT signal */

    		/* Unblock SIGINT */
    		if(sigprocmask(SIG_UNBLOCK, &BlkMsk, NULL) == -1){
      	 		perror("sigprocmask");
        		exit(EXIT_FAILURE);
    	
		}
    		printf("Signals Unblocked,Now uh can perform action.\n");
		/* Print a message indicating that the program is waiting for SIGINT& SIGQUIT signal */
		printf("Waiting for SIGINT signal (Press \"Ctrl+c\" to send SIGINT)...\n");
                printf("Press \"Ctrl+\\\" or \"Ctrl+z\" to quit...\n");
		/* Keep the program running indefinitely, waiting for signals */
    		for(;;){
			/* Block until a signal is caught */
			pause();
		}
	}

    	return 0;
}


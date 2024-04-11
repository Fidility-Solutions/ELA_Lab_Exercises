#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void RT_SigHndlr(int sig){
    	printf("Received real-time signal: %d\n", sig);
}
void RgstrHndlr(int sig){
    	printf("Registering handler for real-time signal %d\n", sig);
    	signal(sig, RT_SigHndlr);
}

void SigGen(int sig){
    	printf("Generating real-time signal %d\n", sig);
    	if(raise(sig) != 0){
        	perror("raise");
        	exit(EXIT_FAILURE);
    	}
    	printf("Signal generated successfully\n");
}

void Blk_UnBlkSignal(int sig){
    	printf("Blocking and unblocking real-time signal %d\n", sig);
    	sigset_t mask;
    	sigemptyset(&mask);
    	sigaddset(&mask, sig);

    	printf("Blocking signal %d\n", sig);
    	if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        	perror("sigprocmask");
        	exit(EXIT_FAILURE);
    	}

    	printf("Signal %d blocked. Press Enter to unblock...\n", sig);
	/* Wait for user to press Enter */
    	getchar();

    	printf("Unblocking signal %d\n", sig);
    	if(sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) {
        	perror("sigprocmask");
        	exit(EXIT_FAILURE);
    	}
    	printf("Signal %d unblocked\n", sig);
}

void SignalSend(int sig){
    	printf("Sending real-time signal %d to the current process\n", sig);
    	if(kill(getpid(), sig) == -1) {
        	perror("kill");
        	exit(EXIT_FAILURE);
    	}
    	printf("Signal sent successfully\n");
}

int main(void){
   	 printf("Select a topic to perform actions:\n");
   	 printf("1. Generate a real-time signal SIGRTMIN\n");
   	 printf("2. Block and unblock a real-time signal SIGRTMIN\n");
   	 printf("3. Send a real-time signal SIGRTMIN\n");
   	 printf("4. Exit\n");

	  printf("Register a handler for real-time signal SIGRTMIN\n");
	 RgstrHndlr(SIGRTMIN);
   	 int choice;
   	 printf("Enter your choice: ");
   	 scanf("%d", &choice);
	 /* Consume newline character */
   	 getchar(); 

   	 switch(choice){
   	     	case 1:
		 	SigGen(SIGRTMIN);
   	      		break;

   	     	case 2:
   	         	Blk_UnBlkSignal(SIGRTMIN);
   	         	break;

   	     	case 3:
   	         	SignalSend(SIGRTMIN);
   	         	break;

		case 4:
   	         	printf("Exiting...\n");
   	         	exit(EXIT_SUCCESS);
   	     
   	     	default:
   	         	printf("Invalid choice. Please enter a number between 1 and 5.\n");
   	         	exit(EXIT_FAILURE);
   	 }

   	 return 0;
}


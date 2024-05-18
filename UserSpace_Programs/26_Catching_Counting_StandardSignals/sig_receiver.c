/* File:        sig_receiver.c
 *
 * Description: Demonstrates catching and counting signals in a standard signal handler.
 *              The program sets up a signal handler for each signal and counts the number 
 *              of times each signal is received.
 *              It can also block signals for a specified amount of time and then display the pending signals.
 *
 *  Usage:      ./sig_receiver <sleep-interval> &
 *
 *  Date: 	25/03/2024
 *
 *  Author:     Fidility Solutions
 *
 *  Reference: "The Linux Programming Interface" book.
*/


#define _GNU_SOURCE
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/* 
 * Function: 	errExit
 *
 * Description: Prints an error message corresponding to the given message and exits the program with failure status.
 * Parameters:
 *      	- msg: A string containing the error message.
 *
 *  Return: void
*/
void errExit(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}
static int sigCnt[NSIG]; /* Counts deliveries of each signal */
static volatile sig_atomic_t gotSigint = 0;
/* Set nonzero if SIGINT is delivered */
/*
 * Function: 	handler
 *
 * Description: Handles signals received by the process. Increments the count of received signals 
 * 		and sets a flag if SIGINT is received.
 *  Parameters:
 *      	- sig: The signal number.
 *
 *  Return: void
*/
static void handler(int sig)
{
	if (sig == SIGINT)
		gotSigint = 1;
	else
		sigCnt[sig]++;
}
/*
 * Function: printSigset
 *
 * Description: Prints the signals set in the given signal set.
 *
 * Parameters:
 *      	- of: A FILE pointer representing the output stream.
 *      	- prefix: A string prefix to be printed before each signal.
 *      	- sigset: A pointer to a sigset_t structure containing the signal set.
 *
 *  Return: void
*/
void printSigset(FILE *of, const char *prefix, const sigset_t *sigset){
	int sig, cnt;
	cnt = 0;
	for(sig = 1; sig < NSIG; sig++){
		if(sigismember(sigset, sig)){
			cnt++;
			fprintf(of, "%s%d (%s)\n", prefix, sig, strsignal(sig));
		}
	}
	if(cnt == 0)
	fprintf(of, "%s<empty signal set>\n", prefix);
}
/*
 * Function: main
 *
 * Description:
 *       	- Entry point of the program.
 *       	- Sets up signal handlers for all signals.
 *       	- Optionally blocks signals for a specified amount of time and displays pending signals.
 *       	- Continuously runs until interrupted by SIGINT.
 *       	- Prints the number of times each signal is caught.
 *
 *  Parameters:
 *      	- argc: An integer representing the number of command-line arguments.
 *      	- argv: An array of strings representing the command-line arguments.
 *
 *  Return: 	int (status code)
*/
int main(int argc, char *argv[]){
	printf("Welcome to catching and counting signals in standard signal\n");
	/*Variable Declaration */
	int n, numSecs;
	sigset_t pendingMask, blockingMask, emptyMask;
	printf("%s: PID is %ld\n", argv[0], (long) getpid());

	for(n = 1; n < NSIG; n++) /* Same handler for all signals */
		(void) signal(n, handler); /* Ignore errors */

		/* If a sleep time was specified, temporarily block all signals,
		sleep (while another process sends us signals), and then
		display the mask of pending signals and unblock all signals */
		if(argc > 1){
			numSecs = atoi(argv[1]);
			sigfillset(&blockingMask);
			if(sigprocmask(SIG_SETMASK, &blockingMask, NULL) == -1)
				errExit("sigprocmask");
			printf("%s: sleeping for %d seconds\n", argv[0], numSecs);
			sleep(numSecs);
			if(sigpending(&pendingMask) == -1)
				errExit("sigpending");
			 printf("%s: Number of pending signals are:\n", argv[0]);
			 printSigset(stdout,"\t\t",&pendingMask);
			sigemptyset(&emptyMask); /* Unblock all signals */
			if(sigprocmask(SIG_SETMASK, &emptyMask, NULL) == -1)
				errExit("sigprocmask");
		}
		while(!gotSigint) /* Loop until SIGINT caught */
			continue;

		for (n = 1; n < NSIG; n++) /* Display number of signals received */
			if(sigCnt[n] != 0)
				printf("%s: signal %d caught %d time%s\n", argv[0], n,sigCnt[n], (sigCnt[n] == 1) ? "" : "s");
		exit(EXIT_SUCCESS);
													
}

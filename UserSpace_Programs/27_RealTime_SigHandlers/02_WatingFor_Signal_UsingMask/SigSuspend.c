#define _GNU_SOURCE /* Get strsignal() declaration from <string.h> */
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

static volatile sig_atomic_t gotSigquit = 0;
void errExit(const char *Msg){
	perror(Msg);
	exit(EXIT_FAILURE);
}
static void handler(int sig){
	printf("Caught signal %d (%s)\n", sig, strsignal(sig));
	/* UNSAFE (see Section 21.1.2) */
	if(sig == SIGQUIT)
		gotSigquit = 1;
}
int main(int argc, char *argv[]){
	printf("This program demonnistrates signal suspend\n");
	/*Variable Declaration */
	int loopNum;
	time_t startTime;
	sigset_t origMask, blockMask;
	struct sigaction sa;
	fprintf(stdout, "Initial signal mask is:\n");
	sigemptyset(&blockMask);
	sigaddset(&blockMask, SIGINT);
	sigaddset(&blockMask, SIGQUIT);
	if(sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1)
		errExit("sigprocmask - SIG_BLOCK");
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if(sigaction(SIGINT, &sa, NULL) == -1)
		errExit("sigaction");
	if(sigaction(SIGQUIT, &sa, NULL) == -1)
		errExit("sigaction");
	for(loopNum = 1; !gotSigquit; loopNum++) {
		printf("=== LOOP %d\n", loopNum);
		/* Simulate a critical section by delaying a few seconds */
		fprintf(stdout, "Starting critical section, signal mask is:\n");
		for(startTime = time(NULL); time(NULL) < startTime + 4; )
			continue; /* Run for a few seconds elapsed time */
		fprintf(stdout,"Before sigsuspend() - pending signals:\n");
		if(sigsuspend(&origMask) == -1 && errno != EINTR)
			errExit("sigsuspend");
	}
	if(sigprocmask(SIG_SETMASK, &origMask, NULL) == -1)
		errExit("sigprocmask - SIG_SETMASK");
	fprintf(stdout, "=== Exited loop\nRestored signal mask to:\n");
	/* Do other processing... */
	exit(EXIT_SUCCESS);
}

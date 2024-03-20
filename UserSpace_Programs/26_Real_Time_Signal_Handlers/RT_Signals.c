#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/signalfd.h>
// Signal handler for real-time signals
void rt_signal_handler(int sig) {
    printf("Received real-time signal: %d\n", sig);
}
/* Handler for signals established using SA_SIGINFO */
static void siginfoHandler(int sig, siginfo_t *si, void *ucontext)
{
	/* UNSAFE: This handler uses non-async-signal-safe functions
	(printf()); see Section 21.1.2) */
	/* SIGINT or SIGTERM can be used to terminate program */
	if (sig == SIGINT || sig == SIGTERM) {
		allDone = 1;
		return;
	}
	sigCnt++;
	printf("caught signal %d\n", sig);
	printf(" si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
	(si->si_code == SI_USER) ? "SI_USER" :
	(si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
	printf("si_value=%d\n", si->si_value.sival_int);
	printf(" si_pid=%ld, si_uid=%ld\n", (long) si->si_pid, (long) si->si_uid);
	sleep(handlerSleepTime);
}
int main(int argc, char *argv[]){
	if (argc < 2 || strcmp(argv[1], "--help") == 0)
		usageErr("%s sig-num...\n", argv[0]);
	/* Display our PID and UID, so that they can be compared with the
	corresponding fields of the siginfo_t argument supplied to the
	handler in the receiving process */
	printf("%s: PID is %ld, UID is %ld\n", argv[0],(long) getpid(), (long) getuid());
    	/* Registering signal handler for real-time signals */
	/* Establish handler for most signals. During execution of the handler,
	mask all other signals to prevent handlers recursively interrupting
	each other (which would make the output hard to read). */
    struct sigaction sa;
    sa.sa_sigaction = siginfoHandler;
    sa.sa_handler = rt_signal_handler;
    sigemptyset(&sa.sa_mask);
  //  sa.sa_flags = SA_RESTART;
    sa.sa_flags = SA_SIGINFO;
    sigfillset(&sa.sa_mask);
    for (sig = 1; sig < NSIG; sig++)
		if (sig != SIGTSTP && sig != SIGQUIT)
			sigaction(sig, &sa, NULL);
//    sigaction(SIGRTMIN, &sa, NULL);

    /* Sending a real-time signal */
    printf("Sending real-time signal SIGRTMIN...\n");
    union sigval sv;
    sv.sival_int = 42; /* Example value to send with the signal */
    sigqueue(getpid(), SIGRTMIN, sv);

    /* Handling real-time signals */
    printf("Waiting for a real-time signal...\n");
    sigset_tprevMask, intMask;
    sigemptyset(&initMask);
    sigaddset(&initMask, SIGINIT);
    sigsuspend(&initMask);

    /* Synchronously waiting for a signal */
    printf("Waiting for a real-time signal synchronously...\n");
    siginfo_t si;
    sigwaitinfo(&initmask, &si);
    printf("Received real-time signal synchronously: %d\n", si.si_signo);

    /* Fetching signals via a file descriptor */
    printf("Fetching signals via a file descriptor...\n");
    sfd = signalfd(-1, &mask, 0);
	if(sfd == -1)
		errExit("signalfd");
	struct signalfd_siginfo fdsi;
    	for (;;){
		int s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
		if(s != sizeof(struct signalfd_siginfo))
			errExit("read");
		printf("%s: got signal %d", argv[0], fdsi.ssi_signo);
		printf("Received real-time signal via file descriptor: %d\n", fdsi.ssi_signo);
		if(fdsi.ssi_code == SI_QUEUE) {
			printf("; ssi_pid = %d; ", fdsi.ssi_pid);
			printf("ssi_int = %d", fdsi.ssi_int);
		}
		printf("\n");
	}
}
    printf("Received real-time signal via file descriptor: %d\n", fdsi.ssi_signo);

    /* Cleanup */
    close(fd);

    return 0;
}


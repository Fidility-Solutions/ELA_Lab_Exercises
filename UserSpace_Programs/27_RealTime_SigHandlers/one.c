#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/signalfd.h>

volatile sig_atomic_t allDone = 0;

void rt_signal_handler(int sig){
    	printf("Received real-time signal: %d\n", sig);
}

static void siginfoHandler(int sig, siginfo_t *si, void *ucontext){
    	if(sig == SIGINT || sig == SIGTERM) {
        	allDone = 1;
   //     	exit(EXIT_FAILURE);
        	return;
    	}

   	printf("Caught signal %d\n", sig);
	/* print the signal details */
    	printf("si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
           (si->si_code == SI_USER) ? "SI_USER" :
           (si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    	printf("si_value=%d\n", si->si_value.sival_int);
    	printf("si_pid=%ld, si_uid=%ld\n", (long) si->si_pid, (long) si->si_uid);
    	sleep(1);
}

int main(int argc, char *argv[]){
    	if(argc < 2 || strcmp(argv[1], "--help") == 0){
        	printf("%s sig-num...\n", argv[0]);
        	exit(EXIT_FAILURE);
    	}

    	printf("%s: PID is %ld, UID is %ld\n", argv[0], (long) getpid(), (long) getuid());

    	/* Registering signal handler for most signals */
	/* Establish handler for most signals. During execution of the handler,
	mask all other signals to prevent handlers recursively interrupting
	each other (which would make the output hard to read). */
    	struct sigaction sa;
    	sa.sa_sigaction = siginfoHandler;
    	sa.sa_flags = SA_SIGINFO;
    	sigemptyset(&sa.sa_mask);

    	for(int sig = 1; sig < NSIG; sig++){
        	if(sig != SIGTSTP && sig != SIGQUIT)
            		sigaction(sig, &sa, NULL);
    	}

    	/* Sending a real-time signal */
    	printf("Press Enter to send a real-time signal SIGRTMIN...\n");
	getchar();
    	printf("Sending real-time signal SIGRTMIN...\n");
    	union sigval sv;
	/* Example value to send with the signal */
    	sv.sival_int = 42; 
    	sigqueue(getpid(), SIGRTMIN, sv);

    	sigset_t initMask,prevMask;
	/* Optionally block signals and sleep, allowing signals to be
	sent to us before they are unblocked and handled */
	if(argc > 1){
    		sigfillset(&initMask);
    		sigdelset(&initMask, SIGQUIT);
    		sigdelset(&initMask, SIGINT);
		sigdelset(&initMask, SIGTERM);
		if(sigprocmask(SIG_SETMASK, &blockMask, &prevMask) == -1){
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}
		printf("%s: signals blocked - sleeping %s seconds\n", argv[0], argv[1]);
		sleep(atoi(argv[1]));
		printf("%s: sleep complete\n", argv[0]);
		if(sigprocmask(SIG_SETMASK, &prevMask, NULL) == -1){
			perror("sigprocmask");
			exit(EXIT_FAILURE);
		}
	}
    	while(!allDone){
		pause();
    	}


    	/* Synchronous signal suspension */
    	printf("Press Ctrl+C to interrupt and suspend execution.\n");
    	printf("Suspending execution until signal reception...\n");
    	sigsuspend(&initMask);
	sigemptyset(&initMask);
	sigaddset(&initMask, SIGINT);
	sigaddset(&blockMask, SIGQUIT);
	if(sigprocmask(SIG_BLOCK, &initMask, &prevMask) == -1){
		perror("sigprocmask - SIG_BLOCK");
		exit(EXIT_FAILURE);
	}
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if(sigaction(SIGINT, &sa, NULL) == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	if(sigaction(SIGQUIT, &sa, NULL) == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
	if(sigsuspend(&origMask) == -1 && errno != EINTR){
		perror("sigsuspend");
		exit(EXIT_FAILURE);
	}
	if(sigprocmask(SIG_SETMASK, &origMask,NULL) == -1){
		perror("sigprocmask - SIG_SETMASK");
		exit(EXIT_FAILURE);
	}
    	printf("Resuming execution after signal reception.\n");

    	/* Handling real-time signals using sigwaitinfo */
	printf("Press Ctrl+C to interrupt and proceed to the next step.\n");
    	printf("Waiting for a real-time signal using sigwaitinfo...\n");
    	siginfo_t si;
    	sig=sigwaitinfo(&initMask, &si);
	if(sig == -1){
		perror("sigwaitinfo");
		exit(EXIT_FAILURE);
	}
    	printf("Received real-time signal: %d\n", si.si_signo);

    	/* Fetching signals via a file descriptor */
    	printf("Press Ctrl+C to interrupt and proceed to the next step.\n");
    	printf("Fetching signals via a file descriptor...\n");
    	int sfd = signalfd(-1, &initMask, 0);
    	if(sfd == -1)
        	perror("signalfd");
    	struct signalfd_siginfo fdsi;
   	printf("Press Ctrl+C to interrupt and terminate the program.\n");
    	printf("Press Enter to continue fetching signals...\n");
    	/* Wait for user to press Enter */
   	 getchar();

    	for(;;){
        	ssize_t s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
        	if(s != sizeof(struct signalfd_siginfo))
            		perror("read");
        	printf("Received real-time signal via file descriptor: %d\n", fdsi.ssi_signo);
        	if(fdsi.ssi_code == SI_QUEUE) {
            		printf("ssi_pid = %d; ssi_int = %d\n", fdsi.ssi_pid, fdsi.ssi_int);
        	}
		 printf("Press Enter to continue fetching signals, or Ctrl+C to terminate the program.\n");
		 /* Wait for user to press Enter */
	        getchar(); 
    	}
	/* Close file descriptor */
    	close(sfd);
    	return 0;
}


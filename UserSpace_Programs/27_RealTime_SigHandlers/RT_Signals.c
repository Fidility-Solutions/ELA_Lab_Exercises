#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/signalfd.h>

volatile sig_atomic_t allDone = 0;

void rt_signal_handler(int sig) {
    printf("Received real-time signal: %d\n", sig);
}

static void siginfoHandler(int sig, siginfo_t *si, void *ucontext) {
    	if(sig == SIGINT || sig == SIGTERM){
	    	printf("Received termination signal: %d\n", sig);
	    	exit(EXIT_FAILURE);
    	}

    	printf("Caught signal %d\n", sig);
    	printf("si_signo=%d, si_code=%d (%s), ", si->si_signo, si->si_code,
           	(si->si_code == SI_USER) ? "SI_USER" :
           	(si->si_code == SI_QUEUE) ? "SI_QUEUE" : "other");
    	printf("si_value=%d\n", si->si_value.sival_int);
    	printf("si_pid=%ld, si_uid=%ld\n", (long) si->si_pid, (long) si->si_uid);
    	sleep(1);
}

int main(int argc, char *argv[]) {
    	printf("Welcome to Signal Handling Program!\n");
    	printf("Choose an operation:\n");
    	printf("1. Sending real-time signal\n");
    	printf("2. Handling real-time signals\n");
    	printf("3. Synchronously waiting for a signal\n");
    	printf("4. Fetching signals via a file descriptor\n");

    	int choice;
    	if(scanf("%d", &choice) != 1) {
        	fprintf(stderr, "Error reading input.\n");
        	exit(EXIT_FAILURE);
    	}

    switch(choice){
	    	case 1:
            		printf("Sending real-time signal SIGRTMIN...\n");
            		union sigval sv;
            		sv.sival_int = 42; /* Example value to send with the signal */
            		if(sigqueue(getpid(), SIGRTMIN, sv) == -1){
		    		perror("sigqueue");
		    		exit(EXIT_FAILURE);
            		}
            		printf("Real-time signal sent.\n");
            		break;

        	case 2:
            		printf("Handling real-time signals...\n");
            		struct sigaction sa;
            		sa.sa_sigaction = siginfoHandler;
            		sa.sa_flags = SA_SIGINFO;
            		sigemptyset(&sa.sa_mask);
            		for(int sig = 1; sig < NSIG; sig++) {
                		if(sig != SIGTSTP && sig != SIGQUIT) {
                    			if(sigaction(sig, &sa, NULL) == -1) {
                        			fprintf(stderr, "Error setting up signal handler for signal %d: %s\n", 
							sig, strerror(errno));
                        			/* Continue with other signals */
                        			continue;
                    			}
                		}
            		}
            		printf("Press Ctrl+C to interrupt and terminate the program.\n");
            		while (!allDone)
                	sleep(1); // Waiting indefinitely
            		break;

        	case 3:
            		printf("Synchronously waiting for a signal...\n");
            		sigset_t initMask;
            		sigfillset(&initMask);
            		sigdelset(&initMask, SIGQUIT);
            		sigdelset(&initMask, SIGINT);
            		int sig;
            		if(sigwait(&initMask, &sig) == -1) {
                		perror("sigwait");
                		exit(EXIT_FAILURE);
            		}
            		printf("Received signal: %d\n", sig);
            		break;

        	case 4:
	    		printf("Fetching signals via a file descriptor...\n");
            		sigset_t mask;
            		sigemptyset(&mask);
	    		for (j = 1; j < argc; j++)
			sigaddset(&mask, atoi(argv[j]));
			if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
				errExit("sigprocmask");
            		sigaddset(&mask, SIGQUIT);
            		sigaddset(&mask, SIGINT);
            		int sfd = signalfd(-1, &mask, 0);
            		if(sfd == -1) {
                		perror("signalfd");
                		exit(EXIT_FAILURE);
            		}
            struct signalfd_siginfo fdsi;
            printf("Press Ctrl+C to interrupt and terminate the program.\n");
            printf("Press Enter to continue fetching signals...\n");
	    printf("ok");
	    printf("Thank you");
            getchar(); /* Wait for user to press Enter */
	    printf("ok");
            while(1){
		    printf("Hello");
		    ssize_t s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
		    if(s == -1){
                   	perror("read");
                    	exit(EXIT_FAILURE);
		    } 
		else if (s != sizeof(struct signalfd_siginfo)) {
                    	fprintf(stderr, "Incomplete read.\n");
                    	exit(EXIT_FAILURE);
                }
                printf("Received real-time signal via file descriptor: %d\n", fdsi.ssi_signo);
                if(fdsi.ssi_code == SI_QUEUE){
                    printf("ssi_pid = %d; ssi_int = %d\n", fdsi.ssi_pid, fdsi.ssi_int);
                }
            }
            close(sfd);
            break;

        default:
            fprintf(stderr, "Invalid choice. Please enter a valid option.\n");
            exit(EXIT_FAILURE);
            break;
    }

    return 0;
}


void fetchSignal(){
	sigset_t mask;
	int sfd, j;
	struct signalfd_siginfo fdsi;
	ssize_t s;
	if(argc < 2 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr,"%s sig-num...\n", argv[0]);
		exit(EXIT_FAILURE);
	}
		
	printf("%s: PID = %ld\n", argv[0], (long) getpid());
	sigemptyset(&mask);
	for(j = 1; j < argc; j++)
		sigaddset(&mask, atoi(argv[j]));
	if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
		errExit("sigprocmask");
	sfd = signalfd(-1, &mask, 0);
	if(sfd == -1)
		errExit("signalfd");
	for(;;){
		s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
		if(s != sizeof(struct signalfd_siginfo))
			errExit("read");
		printf("%s: got signal %d", argv[0], fdsi.ssi_signo);
		if(fdsi.ssi_code == SI_QUEUE) {
			printf("; ssi_pid = %d; ", fdsi.ssi_pid);
			printf("ssi_int = %d", fdsi.ssi_int);
		}
		printf("\n");
	}
}

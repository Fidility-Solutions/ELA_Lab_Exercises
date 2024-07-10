#include <sys/signalfd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
void errExit(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]){
	printf("This program demonistrates fetching of signals\n");
	/*Variable Declaration */
	sigset_t mask;
	int sfd, j;
	struct signalfd_siginfo fdsi;
	ssize_t s;
	if(argc < 2 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr,"%s sig-num...\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	printf("%s: My PID = %ld\n", argv[0], (long) getpid());
	sigemptyset(&mask);
	for(j = 1; j < argc; j++)
		sigaddset(&mask, atoi(argv[j]));
	if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
		errExit("sigprocmask");
	sfd = signalfd(-1, &mask, 0);
	if(sfd == -1)
		errExit("signalfd");
	printf("OK\n");
	for(;;){
		printf("Welcome\n");
		s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
		printf("Fine\n");
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

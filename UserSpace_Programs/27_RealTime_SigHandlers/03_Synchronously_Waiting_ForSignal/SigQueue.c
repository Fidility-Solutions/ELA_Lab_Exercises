#define _POSIX_C_SOURCE 199309
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
void errExit(const char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]){
	printf("This is signal send program\n");
	/* Variable Declaration */
	int sig, numOfSigs, j, sigData;
	union sigval sv;
	/* Check number of arguments */
	if(argc < 4 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr,"%s pid sig-num data [num-sigs]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	/* Display our PID and UID, so that they can be compared with the
	corresponding fields of the siginfo_t argument supplied to the
	handler in the receiving process */
	printf("%s: PID is %ld,and UID is %ld\n", argv[0],(long) getpid(), (long) getuid());
	sig = atoi(argv[2]);
	sigData = atoi(argv[3]);
	numOfSigs = (argc > 4) ? atoi(argv[4]) : 1;
	/* Send signal */
	for(j = 0; j < numOfSigs; j++){
		sv.sival_int = sigData + j;
		if(sigqueue(atoi(argv[1]), sig, sv) == -1){
			fprintf(stderr,"sigqueue %d", j);
			exit(EXIT_FAILURE);
		}
	}
		exit(EXIT_SUCCESS);
}

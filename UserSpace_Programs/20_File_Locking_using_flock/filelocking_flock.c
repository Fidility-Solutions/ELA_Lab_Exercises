#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void errExit(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}
void usageErr(char *msg,const char *var){
	fprintf(stderr,msg,&var);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
	int fd, lock;
	const char *lname;
	time_t CurrTime = time(NULL);
	if(argc < 3 || strcmp(argv[1], "--help") == 0 ||strchr("sx", argv[2][0]) == NULL)
		usageErr("%s file lock [sleep-time]\n"
		"'lock' is 's' (shared) or 'x' (exclusive)\n"
		"optionally followed by 'n' (nonblocking)\n"
		"'secs' specifies time to hold lock\n", argv[0]);

	lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
	if(argv[2][1] == 'n')
		lock |= LOCK_NB;
	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		errExit("open");

	/* Open file to be locked */
	lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";
	printf("PID %ld: requesting %s at %ld\n", (long) getpid(), lname,time(&CurrTime));
	if(flock(fd, lock) == -1){
		if(errno == EWOULDBLOCK)
	fprintf(stderr,"PID %ld: already locked - bye!", (long) getpid());
	else
		fprintf(stderr,"flock (PID=%ld)", (long) getpid());
	}
	printf("PID %ld: granted %s at %ld\n", (long) getpid(), lname,time(&CurrTime));
	sleep((argc == 4) ? atoi(argv[3]) : 10);
	printf("PID %ld: releasing %s at %ld\n", (long) getpid(), lname,time(&CurrTime));
	if(flock(fd, LOCK_UN) == -1)
		errExit("flock");
	exit(EXIT_SUCCESS);
}

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
	printf("Welcome to File Locking concept using flock() system call\n");
	/* variable declaration */
	int fd, lock;
	const char *lname;
	/*To fetch current time */
	time_t CurrTime = time(NULL);
	/* check the number of arguments passed */
	if(argc < 3 || strcmp(argv[1], "--help") == 0 ||strchr("sx", argv[2][0]) == NULL)
		usageErr("%s file lock [sleep-time]\n"
		"'lock' is 's' (shared) or 'x' (exclusive)\n"
		"optionally followed by 'n' (nonblocking)\n"
		"'secs' specifies time to hold lock\n", argv[0]);
	
	/* check the lock reqested from user */
	lock = (argv[2][0] == 's') ? LOCK_SH : LOCK_EX;
	/* Non blocking mode */
	if(argv[2][1] == 'n')
		lock |= LOCK_NB;
	/* open file which is provided from argument */
	fd = open(argv[1], O_RDONLY);
	if(fd == -1)
		errExit("open");

	/* Open file to be locked */
	lname = (lock & LOCK_SH) ? "LOCK_SH" : "LOCK_EX";
	printf("PID %ld: requesting %s at %ld\n", (long) getpid(), lname,time(&CurrTime));
	if(flock(fd, lock) == -1){
		if(errno == EWOULDBLOCK){
			fprintf(stderr,"PID %ld: already locked - bye!\n", (long) getpid());
			exit(EXIT_FAILURE);
		}
		else
			fprintf(stderr,"flock (PID=%ld)", (long) getpid());
	}
	printf("PID %ld: granted %s at %ld\n", (long) getpid(), lname,time(&CurrTime));
	/* sleep time get from argument provided by user */
	sleep((argc == 4) ? atoi(argv[3]) : 10);
	/* release lock */
	printf("PID %ld: releasing %s at %ld\n", (long) getpid(), lname,time(&CurrTime));
	if(flock(fd, LOCK_UN) == -1)
		errExit("flock");
	exit(EXIT_SUCCESS);
}

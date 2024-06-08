/*
 * File name: 	file_locking.c
 *
 * Description: This program demonstrates file locking using the flock() system call in C.
 *            	It allows a process to request a lock on a file, either shared or exclusive, 
 *            	optionally with a non-blocking mode. The program holds the lock for a specified
 *            	duration (default 10 seconds) and then releases it.
 *
 * Usage: 	./file_locking <filename> <lock_type> [sleep-time]
 *      	lock_type can be 's' (shared) or 'x' (exclusive), optionally followed by 'n' (nonblocking)
 *      	sleep-time specifies the time to hold the lock (optional, default is 10 seconds)
 *
 * Author: 	Fidility Solutions
 *
 * Date: 	10/03/2024
 *
 * Reference: 	"The Linux Programming Interface" book
*/
#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
/* 
 * Function:    errExit
 * ------------------
 * Description: Prints the error message corresponding to the given system error number using perror(), 
 *              and exits the program.
 *
 * Parameters:  message - The error message to be printed.
 *
 * Returns:     None
 */

void errExit(char *msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

/*
 * Function name: main
 *
 * Description: Entry point of the program. Parses command line arguments, requests file locks,
 *            	holds them for a specified duration, and releases them.
 * Parameters:
 *   		argc: 	Number of command line arguments
 *   		argv: 	Array of command line arguments
 *
 * Return: 	int
*/
int main(int argc, char *argv[]){
	/* variable declaration */
	int fd, lock;

	const char *lname;

	char *parray = "Hi there How are you";

	/*To fetch current time */
	time_t CurrTime = time(NULL);
	/* check the number of arguments passed */
	if(argc < 3 || strcmp(argv[1], "--help") == 0 ||strchr("sx", argv[2][0]) == NULL){
		fprintf(stderr,"\nSyntax 		: %s 'file' 'lock' [sleep-time]\n"
		"\nfile		: file to lock\n"		
		"lock		: 's' (shared) or 'x' (exclusive)\n"
		"	  	  optionally followed by 'n' (nonblocking)\n"
		"[sleep-time] 	: specifies time to hold lock\n", argv[0]);
		exit(EXIT_FAILURE);
	}


	printf("Welcome to File Locking concept using flock() system call\n");
	
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
	printf("\n");
	printf("PID %ld: requesting %s at %s sec\n", (long) getpid(), lname,ctime(&CurrTime));
	if(flock(fd, lock) == -1){
		if(errno == EWOULDBLOCK){
			fprintf(stderr,"PID %ld: already locked - bye!\n", (long) getpid());
			exit(EXIT_FAILURE);
		}
		else
			fprintf(stderr,"flock (PID=%ld)", (long) getpid());
	}
	printf("PID %ld: granted %s at %s sec\n", (long) getpid(), lname,ctime(&CurrTime));
	fprintf(fd, "%s", parray);
	/* sleep time get from argument provided by user */
	sleep((argc == 4) ? atoi(argv[3]) : 10);
	/* release lock */
	printf("PID %ld: releasing %s at %s\n", (long) getpid(), lname,ctime(&CurrTime));
	if(flock(fd, LOCK_UN) == -1)
		errExit("flock");
	exit(EXIT_SUCCESS);
}

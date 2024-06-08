#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 10
int main(int argc, char *argv[]){
	printf("This program demonistrates communicaion between parent and child using pipe\n");
	int pfd[2];
	char buf[BUF_SIZE];
	ssize_t numRead;
	/* Pipe file descriptors */
	if (argc != 2 || strcmp(argv[1], "--help") == 0){
		fprintf(stderr,"Usage: %s 'Enter data'\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	if (pipe(pfd) == -1){
		perror("pipe fail");
		exit(EXIT_FAILURE);
	}

	switch (fork()){
		case -1:
			perror("fork failed");
			exit(EXIT_FAILURE);
		/* Create the pipe */
		case 0: /* Child - reads from pipe */
			/* Write end is unused */
			printf("\nThis is child process_%d\n",getpid());
			if(close(pfd[1]) == -1){
				perror("close - child");
				exit(EXIT_FAILURE);
			}

			write(STDOUT_FILENO,"Data from parent is:",sizeof("Data from parent is:"));
			while ((numRead = read(pfd[0], buf, BUF_SIZE)) > 0) {
                		if(write(STDOUT_FILENO, buf, numRead) != numRead) {
					perror("write - child");
                    			exit(EXIT_FAILURE);
				}
			 }
			if (numRead == -1) {
                		perror("read");
                		exit(EXIT_FAILURE);
            		}
			write(STDOUT_FILENO, "\n", 1);
			if(close(pfd[0]) == -1){
				perror("close failed");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		default:/* Parent - writes to pipe */
			printf("\nThis is parent_%d\n",getpid());
			if(close(pfd[0]) == -1){/* Read end is unused */
				perror("close - parent failed");
				exit(EXIT_FAILURE);
			}
			printf("Parent written data to pipe:%s\n",argv[1]);	
			if(write(pfd[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
				fprintf(stderr,"parent - partial/failed write");
			if(close(pfd[1]) == -1){/* Child will see EOF */
				perror("close failed");
				exit(EXIT_FAILURE);
			}
			wait(NULL);/* Wait for child to finish */
			exit(EXIT_SUCCESS);
	}
}


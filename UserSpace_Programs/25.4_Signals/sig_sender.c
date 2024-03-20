#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[]){
	if(argc<4 || strcmp(argv[1],"--help") == 0){
		fprintf(stderr,"Usage:%s <PID> <number_of_signal> <signal_number> [additional_signal]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	/* convert string pid to integer */
	pid_t pid=atoi(argv[1]);
	/*No.of sinals to send */
	int Numsigs = atoi(argv[2]);
	/* signal number to send initially */
	int SigNum = atoi(argv[3]);

	/*send specified signals to the target process */
	for(int i=0;i<Numsigs;i++){
		if(kill(pid,SigNum) == -1){
			perror("kill ");
			exit(EXIT_FAILURE);
		}
		printf("Sent signal %d to process %d\n",SigNum,(int)pid);
		/* sleep for 1 sec between signals */
		sleep(1);
	}
	/* If an additional signal is provided then send it to the target process */
	if(argc>4){
		int AddtnSinal = atoi(argv[4]);
		if(kill(pid,AddtnSinal) == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}
		printf("Sent additional signal %d to process %d\n",AddtnSinal,(int)pid);
	}
	return 0;
}

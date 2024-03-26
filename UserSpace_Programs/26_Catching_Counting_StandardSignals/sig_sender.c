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
	/* Send signal to reciever */
	printf("%s: sending signal to %d process %ld %d times\n",argv[0],SigNum,(long)pid,Numsigs);

	/*send specified signals to the target process */
	for(int i=0;i<Numsigs;i++){
		if(kill(pid,SigNum) == -1){
			perror("kill ");
			exit(EXIT_FAILURE);
		}
	}
	/* If an additional signal is provided then send it to the target process */
	int AddtnSinal;
	if(argc>4){
		AddtnSinal = atoi(argv[4]);
		if(kill(pid,AddtnSinal) == -1){
			perror("kill");
			exit(EXIT_FAILURE);
		}
	}
	printf("Sent additional signal %d to process %d\n",AddtnSinal,(int)pid);
	printf("%s:exiting..\n",argv[0]);
	exit(EXIT_SUCCESS);
}

/* 
 * File: 	sig_sender.c
 *
 * Description: Sends specified signals to a target process identified by its PID. 
 *              Optionally sends an additional signal to the target process.
 *              This program provides a command-line interface to send signals to a specified process.
 * Usage: 
 *         	./sig_sender <PID> <number_of_signals> <signal_number> [additional_signal]
 *
 *         	If the "--help" option is used or if the number of arguments is less than 4, 
 *         	it displays the usage message.
 *
 * Date:	25/03/2024.
 *
 * Author: 	Fidility Solutions
 *
 * Reference: 	The "Linux Programming Interface" book.
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
/*
 * Function:	 main
 *
 * Description:
 *       	- Entry point of the program.
 *       	- Parses command-line arguments to extract PID, number of signals, and signal numbers.
 *       	- Sends the specified signals to the target process.
 *       	- Optionally sends an additional signal to the target process.
 *
 * Parameters:
 *      	- argc: An integer representing the number of command-line arguments.
 *      	- argv: An array of strings representing the command-line arguments.
 *
 *  Return: 	int (status code)
*/
int main(int argc, char *argv[]){
	if(argc<4 || strcmp(argv[1],"--help") == 0){
		fprintf(stderr,"Usage:%s <PID> <number_of_signal> <signal_number> [additional_signal]\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	/* convert string pid to integer */
	pid_t pid=atoi(argv[1]);
	/*No.of sinals to send */
	int NumOfsigs = atoi(argv[2]);
	/* signal number to send initially */
	int SigNum = atoi(argv[3]);
	/* Send signal to reciever */
	printf("%s: sending signal to %d process %ld %d times\n",argv[0],SigNum,(long)pid,NumOfsigs);

	/*send specified signals to the target process */
	for(int i=0;i<NumOfsigs;i++){
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

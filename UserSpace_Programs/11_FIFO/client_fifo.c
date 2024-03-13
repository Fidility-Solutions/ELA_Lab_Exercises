#include "common.h" 
/*******************************************************************************************************
 * File:	client_fifo.c
 *
 * Description: Client program for a server-client application using FIFOs.This program sends a request to 
 * 		the server through a FIFO (named pipe) and receives a response from the server.
 *
 * Usage: 	client_fifo [ReqNum...]
 *
 * Author: 	Fidility Solutions
 *
 * Date: 	04/03/2024
 *
 * Reference: 	The Linux Programming Interface book.
 *
 *****************************************************************************************************/
static char clientFifo[CLIENT_FIFO_NAME_LEN];

/* Invoked on exit to delete client FIFO */
/*
Function Name: removeFifo
Description: Invoked on exit to delete the client FIFO.
*/
static void removeFifo(void)
{
	//printf("Client FIFO removed\n");
	unlink(clientFifo);
	//printf("Client FiFO removed\n");
}

/* 
 * Function name: main
 *
 * Descripition: Entry point of the program. Responsible for handling command-line arguments, 
 * 		 creating the client FIFO, sending requests to the server, and receiving responses.
 *
 * Parameters:	- argc: Number of command-line arguments.
 * 		- argv: Array of pointers to command-line arguments.
 *
 * Sending:
 * 		- Constructs a request message containing the process ID and request number.
 * 		- Creates a client FIFO and sends the request to the server through the server FIFO.
 * Receiving:	- Opens the client FIFO to receive the response from the server
 * 		- Reads the response message from the client FIFO and prints the response from the server.
 *
 * Returns:EXIT_SUCCESS: Program execution completed successfully.
 *
*/
int main(int argc, char *argv[]){
	printf("Welcome to server-client application using FIFOs & This is client FIFO\n");
	/* variable declaration */
	int serverFd, clientFd;
	struct request req;
	struct response resp;
	/* check the passed argument if user want any help */
	if (argc > 1 && strcmp(argv[1], "--help") == 0)
		fprintf(stderr,"%s [ReqNum...]\n", argv[0]);
        /* check the passed arguments for ReqNum */
	else{
		if(argc >1){
                	char *endptr;
                	long int value = strtol(argv[1], &endptr, 10);
                	if(*endptr == '\0' && value > 0) {
                        	req.ReqNum = (int)value;
                	}
                	else
                	fprintf(stderr, "Invalid sequence length. Using default value.\n");
		}
		else{
			printf("Enter request number to send to server:");
			scanf("%d",&req.ReqNum);
		}


        }
	/* Construct request message, open server FIFO, and send request */
        req.pid = getpid();

	/* Create our FIFO (before sending request, to avoid a race) */
	/* So we get the permissions we want */
	umask(0);
	snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,(long) getpid());
	if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
		fprintf(stderr,"mkfifo %s", clientFifo);
	
	/* Registering remove client fifo, this will call at end of the program*/
	if (atexit(removeFifo) != 0)
		perror("atexit");
	serverFd = open(SERVER_FIFO, O_WRONLY);
	if (serverFd == -1)
		fprintf(stderr,"open %s", SERVER_FIFO);

	/* sending requst to server fifo */
	if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
		perror("Can't write to server");
	
	/* Open client FIFO, read and display response if server wirte any response to client */
	clientFd = open(clientFifo, O_RDONLY);
	if(clientFd == -1)
		fprintf(stderr,"open %s", clientFifo);

	if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
		perror("Can't read response from server");

	printf("Response from server:%d\n", resp.RespNum);
	exit(EXIT_SUCCESS);
}

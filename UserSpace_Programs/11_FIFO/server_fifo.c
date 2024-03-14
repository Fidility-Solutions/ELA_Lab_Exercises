
/******************************************************************************

 * File:        server_fifo.c
 *
 * Description: This program serves as a server in a client-server application using FIFO.
 *              It creates a server FIFO for inter-process communication, listens for
 *              requests from clients, process the requests, and responds back
 *              to the clients.
 *
 * Usage:       ./server_fifo
 * 
 * Author:      Fidility Solutions.
 *  
 * Date:        04/03/2024.
 *
 * Reference:   The Linux Programming Interface book.

 ******************************************************************************/

#include <signal.h>
#include "common.h"

/*
 * Function: 	main
 * 
 * Description: Entry point of the server program. Creates and initializes the server FIFO,
 *              listens for requests from clients, process the requests and
 *              responds back to the clients.
 *
 * Arguments:   None
 *
 * Returns:     0 upon successful execution of the program.
 */

int main(int argc, char *argv[])
{
	printf("Welcome to server-client application using FIFO\n");
	/* variable declaration*/
	int serverFd, dummyFd, clientFd;
	char clientFifo[CLIENT_FIFO_NAME_LEN];
	struct request req;
	struct response resp;
	resp.RespNum = 0;

	/* Create well-known FIFO, and open it for reading */
	/* Using umask(0) we can get the permissions needed */
	umask(0);
	/* creating server FIFO */
	printf("Server FIFO is created\n");
	if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST){
		fprintf(stderr,"mkfifo %s", SERVER_FIFO);
		exit(EXIT_FAILURE);
	}

	/* Open server fifo to Read the request from client */
	printf("Server FIFO opened to read from clients\n");
	serverFd = open(SERVER_FIFO, O_RDONLY);
	if (serverFd == -1){
		fprintf(stderr,"open %s", SERVER_FIFO);
		exit(EXIT_FAILURE);
	}
	/* Open an extra write descriptor, so that we never see EOF */
	dummyFd = open(SERVER_FIFO, O_WRONLY);
	if (dummyFd == -1)
		fprintf(stderr,"open %s", SERVER_FIFO);
	/* Ignore SIGPIPE signal to prevent process termination 
	 * SIGPIPE is generated when writing to a closed FIFO (named pipe).
	 * By ignoring SIGPIPE, we ensure that the process does not terminate
	 * if the writing end of the FIFO is closed before all data is written.
	 */

	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		perror("signal");
	for(;;){ 	
		/* Read requests and send responses */
		if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)){
			fprintf(stderr, "Error reading request; discarding\n");
			continue;
		}
		/* Open client FIFO (previously createstrtold by client) to write only mode */
		printf("Received data from client(PID %d): Request Number %d\n", req.pid, req.ReqNum);
		snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,(long) req.pid);
		clientFd = open(clientFifo, O_WRONLY);
		
		/* Open failed, give up on client */
		if(clientFd == -1){ 
			fprintf(stderr,"open %s", clientFifo);
			continue;
		}
		/* Send response and close FIFO */
		resp.RespNum = resp.RespNum+req.ReqNum;

		if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
			fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
		printf("Sent response to the client\n");
		resp.RespNum=0;
		/*close client fifo */
	if (close(clientFd) == -1)
		perror("close");
	}
}

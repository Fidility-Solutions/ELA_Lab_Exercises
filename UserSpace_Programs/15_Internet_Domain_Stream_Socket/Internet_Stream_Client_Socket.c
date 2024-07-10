/**************************************************************************
 * File:        Internet_Stream_Client_Socket.c
 *
 * Description: This program demonstrates a simple Internet domain stream socket client using the socket API. 
 * 		It creates a client socket, connect to server and send & receive data from server.
 *
 * Usage:       ./Internet_Stream_Client_Socket
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        02/03/2024
 *
 * Reference:   The "Linux Programming Interface" book.
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>

#define PORT_NUM "9640"
#define BUFFER_SIZE 50
#define SERVER_IP "10.10.1.27"
/* Function: main
 *
 * Description: Entry point of the client program. Creates a Internet domain stream socket client,
 *               connect to the server with specified ip address and port number, transmits and receives response from/to server,
 *              and closes the socket if any error occurs.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main(void) {
	printf("\nwelcome to client application program In Intenet Domain stream socket\n");
	/*variable Declaration */
    	int SktFd;
    	char as8Buffer[BUFFER_SIZE];
	ssize_t NumRead;
	struct addrinfo hints;
	struct addrinfo *result, *rp;

    	/* Initialize hints */
    	memset(&hints, 0, sizeof(hints));
	/* Allow IPV4 or IPV6 */
    	hints.ai_family = AF_UNSPEC; 
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;
    	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_NUMERICSERV;

    	/* Get address info for TCP server */
    	if (getaddrinfo(SERVER_IP, PORT_NUM, &hints, &result) != 0) {
        	perror("getaddrinfo error");
        	exit(EXIT_FAILURE);
    	}
	/* Walk through returned list until we find an address structure
	that can be used to successfully connect a socket */
    	for (rp = result; rp != NULL; rp = rp->ai_next) {
        	/* Create socket */
        	if ((SktFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            		perror("socket");
            		continue;
        	}

        	/* Connect to server */
        	if (connect(SktFd, rp->ai_addr, rp->ai_addrlen)!= -1) {
			/* Connection successful */
            		break;	
        	}
    	}


    	/* Check if connection was successful */
    	if (rp == NULL) {
        	perror("Failed to connect\n");
		exit(EXIT_FAILURE);
	}
        
	/* Free address info */
        freeaddrinfo(result);
	while(1){
		const char *message = "Hello, server!";
		/* Send the length of desired sequence to server */
		if(send(SktFd, message, strlen(message),0) == -1){
			/* Connection closed by server */
			if (errno == EPIPE) {
	        		fprintf(stderr, "Server disconnected\n");
	        		break;
	        	} 
			else {
	        		perror("send fail");
	        		exit(EXIT_FAILURE);
	        	}
		}
		printf("Sent data to server: %s\n", message);
		
		/* Receive response from server */
		int recvbytes = recv(SktFd, as8Buffer, BUFFER_SIZE - 1, 0);
		if(recvbytes == -1){
			/* Connection reset by server */
			if (errno == ECONNRESET) { 
	        		fprintf(stderr, "Server reset the connection\n");
	        		break;
	        	} 
			else {
	        		perror("recv fail");
	        		exit(EXIT_FAILURE);
	        	}
		}
		/* Null-terminate received data */
		as8Buffer[recvbytes] = '\0';
		
		/* Print the data received from server */
		printf("Received from server: %s\n", as8Buffer);
		/* Sleep for 0.5 seconds */
		usleep(500000);
	}
    	/* Close socket */
	close(SktFd);
    	return 0;
}


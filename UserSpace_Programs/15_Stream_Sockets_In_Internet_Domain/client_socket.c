/**************************************************************************
 * File:        client_socket.c
 *
 * Description: This program demonstrates a simple Internet domain Internet socket client
 *              using the socket API. It creates a client socket, binds it to
 *              a Unix domain socket path, listens for incoming connections,
 *              accepts client connections, receives messages from clients,
 *              echoes the messages back to clients, and finally closes the
 *              server socket.
 *
 * Usage:       ./client_socket.c
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        02/03/2024
 *
 * Reference:   The Linux Programming Interface book.
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define PORT_NUM "9640"
#define BUFFER_SIZE 50
/* Function: main
 *
 * Description: Entry point of the server program. Creates a Internet domain stream socket client,
 *              accepts client connections, receives and echoes messages from/to clients,
 *              and closes the server socket.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main() {
	printf("welcome to server-client application program In Intenet Domain stream socket\n");
	/*variable Declaration */
    	int sockfd;
    	char buffer[BUFFER_SIZE];
    	int seqLen;
	char *reqLenStr; /* Requested length of sequence */
	char seqNumStr[BUFFER_SIZE]; /* Start of granted sequence */
	ssize_t numRead;
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
    	if (getaddrinfo("localhost", PORT_NUM, &hints, &result) != 0) {
        	perror("getaddrinfo");
        	exit(EXIT_FAILURE);
    	}
	/* Walk through returned list until we find an address structure
	that can be used to successfully connect a socket */
    	for (rp = result; rp != NULL; rp = rp->ai_next) {
        	/* Create socket */
        	if ((sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            		perror("socket");
            		continue;
        	}

        	/* Connect to server */
        	if (connect(sockfd, rp->ai_addr, rp->ai_addrlen)!= -1) {
            		perror("connect");
            		break;
        	}
		/* Connect failed: close this socket and try next address */
		close(sockfd);
    	}


    	/* Check if connection was successful */
    	if (rp == NULL) {
        	fprintf(stderr, "Failed to connect\n");
//	exit(EXIT_FAILURE);
	}
        
	/* Free address info */
        freeaddrinfo(result);

    	printf("Connected to server on port %s\n", PORT_NUM);

    	/* Ask client for the length of desired sequence */
    	printf("Enter the length of desired sequence: ");
    	scanf("%d", &seqLen);

    	/* Send the length of desired sequence to server */
    	sprintf(buffer, "%d\n", seqLen);
    	if(write(sockfd, buffer, strlen(buffer))!=strlen(buffer))
		fprintf(stderr,"Partial/failed write (newline)");
	/* Read and display sequence number returned by server */
    	/* Receive sequence number from server */
    	int num_bytes = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    	if (num_bytes == -1) 
        	perror("recv");
	if(num_bytes == 0)
		fprintf(stderr,"Unexpected EOF from server");

    	/* Null-terminate received data */
    	buffer[num_bytes] = '\0';

    	/* Print sequence number received from server */
    	printf("Received sequence number from server: %s\n", buffer);

    	/* Close socket */
    	exit(EXIT_SUCCESS);

    return 0;
}


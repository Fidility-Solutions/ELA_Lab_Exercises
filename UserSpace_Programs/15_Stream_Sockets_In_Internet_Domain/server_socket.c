/**************************************************************************
 * File:        server_socket.c
 *
 * Description: This program demonstrates a simple Internet domain Datagram socket server
 *              using the socket API. It creates a server socket, binds it to
 *              a Unix domain socket path, listens for incoming connections,
 *              accepts client connections, receives messages from clients,
 *              echoes the messages back to clients, and finally closes the
 *              server socket.
 *
 * Usage:       ./server_socket.c
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        01/03/2024
 *
 * Reference:   The Linux Programming Interface book.
 *
 ******************************************************************************/
#include<stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

#define PORT_NUM "9640"
#define BUFFER_SIZE 50

#define ADDRSTRLEN 4096

int main(void){
	printf("welcome to server-client application program In Intenet Domain stream socket\n");
	/*variable Declaration */
    	struct addrinfo hints, *res, *p;
    	int s8SrvrFd, s8ClntFd,optval;
    	struct sockaddr_storage strClntAddr;
    	socklen_t AddrLen;
	char as8Buffer[BUFFER_SIZE];
	char as8Host[NI_MAXHOST];
	char as8Service[NI_MAXSERV];
	char as8SeqNum[BUFFER_SIZE];
	char as8Addr[ADDRSTRLEN];
    	uint32_t u32SrvrSeqNum=0;
    	/* Initialize hints */
    	memset(&hints, 0, sizeof(hints));
	/* AF_UNSPEC allows IPV4 or IPV6 */
    	hints.ai_family = AF_UNSPEC;
    	hints.ai_socktype = SOCK_STREAM;
	/*use local Ip address */
    	hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV; // Use my IP address

    	/* Get address info for TCP socket */
    	if (getaddrinfo(NULL, PORT_NUM, &hints, &res) != 0) {
        	perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}
    	
	optval = 1;
    	/* Iterate through address info */
    	for (p = res; p != NULL; p = p->ai_next) {
        	/* Create socket */
        	if ((s8SrvrFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            		perror("socket error");
			printf("socket error number: %d\n", errno);
            		continue;/* On error, try next address */
        	}
		printf("The server socket is created\n");

        	/* Set socket option to reuse address */
        	if (setsockopt(s8SrvrFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            		perror("setsockopt error");
			printf("setsockopt error number: %d\n", errno);
            		exit(EXIT_FAILURE);
       	 	}

        	/* Bind socket to address */
        	if (bind(s8SrvrFd, p->ai_addr, p->ai_addrlen) == -1) {
            		perror("bind");
			printf("bind error number: %d\n", errno);
            		break;
        	}
		printf("The server socket Bind with specified Atrributes\n");
		close(s8SrvrFd);
	}

    	/* Check if binding was successful */
    	if (p == NULL) {
        	fprintf(stderr, "Failed to bind\n");
    	}
	/* Listen for incoming connections */
	printf("Before listen: sockfd = %d\n", s8SrvrFd); // Add this line for debugging
    	/* Listen for incoming connections */
    	if (listen(s8SrvrFd, 50) == -1) {
		printf("listen error number: %d\n", errno); // Print the error number
		perror("listen fail");
    	}
	printf("After listen\n"); // Add this line for debugging

    	/* Free address info */
    	freeaddrinfo(res);
    	printf("Server listening on port %s...\n", PORT_NUM);

    	/* Accept and handle clients incoming connections iteratively */
    	for (;;) { 
        	AddrLen = sizeof(strClntAddr);
		/* Accept a client connection, obtaining client's address */
        	s8ClntFd = accept(s8SrvrFd, (struct sockaddr *)&strClntAddr, &AddrLen);
        	if (s8ClntFd == -1) {
        		perror("accept failed");
        		continue;
        	}
		if (getnameinfo((struct sockaddr *) &strClntAddr, AddrLen,as8Host,NI_MAXHOST, as8Service, NI_MAXSERV, 0) == 0)
			snprintf(as8Addr, ADDRSTRLEN, "(%s, %s)", as8Host, as8Service);
		else
			snprintf(as8Addr, ADDRSTRLEN, "(?UNKNOWN?)");
		printf("Connection from %s\n", as8Addr);
		/* Read client request, send sequence number back */
        	int s8BytesRecv = recv(s8ClntFd, as8Buffer, BUFFER_SIZE - 1, 0);
        	if (s8BytesRecv == -1) {
        		perror("recieve error");
        		close(s8ClntFd);
        		continue;
        	}

        	/* Null-terminate received data */
        	as8Buffer[s8BytesRecv] = '\0';
        	/* Convert client's message to integer */
        	int reqLen = atoi(as8Buffer);
		if (reqLen <= 0) { /* Watch for misbehaving clients */
			close(s8ClntFd);
			continue; /* Bad request; skip it */
		}	
		
        	/* Send response to client with current sequence number */
        	sprintf(as8SeqNum, "%d\n", u32SrvrSeqNum);
        	if(send(s8ClntFd, as8SeqNum, strlen(as8SeqNum),0) != strlen(as8SeqNum))
			fprintf(stderr, "Error on write");

        /* Update sequence number */
        u32SrvrSeqNum += reqLen;

        /* Print client's address */
        printf("Client connected from address: %s\n", inet_ntoa(((struct sockaddr_in *)&strClntAddr)->sin_addr));

        close(s8ClntFd);
    	}

    	/* Close server socket */

    	return 0;
}


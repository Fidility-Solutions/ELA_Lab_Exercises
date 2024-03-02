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
#include <arpa/inet.h> // Include this header for inet_ntoa()
#include <errno.h>
#define PORT_NUM "9640"
#define BUFFER_SIZE 50
#define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV + 10)

int main() {
	printf("welcome to server-client application program In Intenet Domain stream socket\n");
	/*variable Declaration */
    	struct addrinfo hints, *res, *p;
    	int sockfd, clientfd,optval;
    	struct sockaddr_storage client_addr;
    	socklen_t addr_len;
	char buffer[BUFFER_SIZE];
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];
	char seqNumStr[BUFFER_SIZE];
	char addrStr[ADDRSTRLEN];
    	uint32_t seqNum=0;
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
        	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            		perror("socket");
			printf("socket error number: %d\n", errno); // Print the error number
            		continue;/* On error, try next address */
        	}

        	/* Set socket option to reuse address */
        	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
            		perror("setsockopt");
			printf("setsockopt error number: %d\n", errno); // Print the error number
            		exit(EXIT_FAILURE);
       	 	}

        	/* Bind socket to address */
        	if (bind(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
            		perror("bind");
			printf("bind error number: %d\n", errno); // Print the error number
//			close(sockfd); // Close the socket in case of bind failure
            		break;
        	}
		/* bind() failed: close this socket and try next address */
		close(sockfd);
	}

    	/* Check if binding was successful */
    	if (p == NULL) {
        	fprintf(stderr, "Failed to bind\n");
    	}
	/* Listen for incoming connections */
	printf("Before listen: sockfd = %d\n", sockfd); // Add this line for debugging
    	/* Listen for incoming connections */
    	if (listen(sockfd, 50) == -1) {
        	perror("listen");
		 printf("listen error number: %d\n", errno); // Print the error number
        	exit(EXIT_FAILURE);
    	}
	printf("After listen\n"); // Add this line for debugging

    	/* Free address info */
    	freeaddrinfo(res);
    	printf("Server listening on port %s...\n", PORT_NUM);

    	/* Accept and handle clients incoming connections iteratively */
    	for (;;) { 
        	addr_len = sizeof(client_addr);
		/* Accept a client connection, obtaining client's address */
        	clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
        	if (clientfd == -1) {
        		perror("accept");
        		continue;
        	}
		if (getnameinfo((struct sockaddr *) &client_addr, addr_len,host,NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
			snprintf(addrStr, ADDRSTRLEN, "(%s, %s)", host, service);
		else
			snprintf(addrStr, ADDRSTRLEN, "(?UNKNOWN?)");
		printf("Connection from %s\n", addrStr);
		/* Read client request, send sequence number back */
        	int num_bytes = recv(clientfd, buffer, BUFFER_SIZE - 1, 0);
        	if (num_bytes == -1) {
        		perror("recv");
        		close(clientfd);
        		continue;
        	}

        	/* Null-terminate received data */
        	buffer[num_bytes] = '\0';
        	/* Convert client's message to integer */
        	int reqLen = atoi(buffer);
		if (reqLen <= 0) { /* Watch for misbehaving clients */
			close(clientfd);
			continue; /* Bad request; skip it */
		}	
		
        	/* Send response to client with current sequence number */
        	sprintf(seqNumStr, "%d\n", seqNum);
        	if(send(clientfd, seqNumStr, strlen(seqNumStr),0) != strlen(seqNumStr))
			fprintf(stderr, "Error on write");

        /* Update sequence number */
        seqNum += reqLen;
//        seqNum++;

        /* Print client's address */
        printf("Client connected from address: %s\n", inet_ntoa(((struct sockaddr_in *)&client_addr)->sin_addr));

        close(clientfd);
    	}

    	/* Close server socket */
//    	close(sockfd);

    	return 0;
}


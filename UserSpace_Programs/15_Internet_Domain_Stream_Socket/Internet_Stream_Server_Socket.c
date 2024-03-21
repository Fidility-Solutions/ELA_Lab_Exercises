/**************************************************************************
 * File:        server_socket.c
 *
 * Description: This program demonstrates a simple Internet domain Datagram socket server using the socket API. 
 * 		It creates a server socket, binds it to a Unix domain socket path, listens for incoming connections,
 *              accepts client connections, receives messages from clients, echoes the messages back to clients, and 
 *              finally closes the server socket.
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

#define PORT_NUM "96400"
#define BUFFER_SIZE 50

#define ADDRSTRLEN 4096
int factNum(int Num);
int main(void){
	printf("welcome to server-client application program In Intenet Domain stream socket\n");
	/*variable Declaration */
    	struct addrinfo hints, *res, *p;
    	int SrvrFd, ClntFd,optval;
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
    	hints.ai_flags = AI_PASSIVE|AI_NUMERICSERV;

    	/* Get address info for TCP socket */
    	if (getaddrinfo(NULL, PORT_NUM, &hints, &res) != 0) {
        	perror("getaddrinfo");
		exit(EXIT_FAILURE);
	}
    	
	optval = 1;

    	/* Iterate through address info */
    	for(p = res; p != NULL; p = p->ai_next){
        	/* Create socket */
        	if((SrvrFd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            		perror("socket error");
			printf("socket error number: %d\n", errno);
            		continue;/* On error, try next address */
        	}
		printf("The server socket is created\n");

        	/* Set socket option to reuse address */
        	if(setsockopt(SrvrFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
            		perror("setsockopt error");
			printf("setsockopt error number: %d\n", errno);
            		exit(EXIT_FAILURE);
       	 	}

        	/* Bind socket to address */
        	if(bind(SrvrFd, p->ai_addr, p->ai_addrlen) == -1){
            		perror("bind");
			printf("bind error number: %d\n", errno);
            		break;
        	}
		fflush(stdout);
		printf("The server socket Bind with specified Atrributes\n");
		break;
	}

    	/* Check if binding was successful */
    	if(p == NULL){
        	fprintf(stderr, "Failed to bind\n");
    	}
	/* Listen for incoming connections */
    	if(listen(SrvrFd, 50) == -1){
		printf("listen error number: %d\n", errno); 
		perror("listen fail");
    	}

    	/* Free address info */
    	freeaddrinfo(res);
    	printf("\nServer listening on port %s...\n", PORT_NUM);

    	/* Accept and handle clients incoming connections iteratively */
    	for(;;){ 
        	AddrLen = sizeof(strClntAddr);

		/* Accept a client connection, obtaining client's address */
        	ClntFd = accept(SrvrFd, (struct sockaddr *)&strClntAddr, &AddrLen);
        	if (ClntFd == -1) {
        		perror("accept failed");
        		continue;
        	}

		/* Print client's address */
		if(getnameinfo((struct sockaddr *) &strClntAddr, AddrLen,as8Host,NI_MAXHOST, as8Service, NI_MAXSERV, 0) == 0)
			snprintf(as8Addr, ADDRSTRLEN, "(%s, %s)", as8Host, as8Service);
		else
			snprintf(as8Addr, ADDRSTRLEN, "(?UNKNOWN?)");

		printf("Connection Established from %s client\n", as8Addr);

		/* Read client request, send sequence number back */
        	int s8BytesRecv = recv(ClntFd, as8Buffer, BUFFER_SIZE - 1, 0);
        	if(s8BytesRecv == -1){
        		perror("recieve error");
        		close(ClntFd);
        		continue;
        	}

		printf("\nData received from client is %s\n",as8Buffer);

        	/* Null-terminate received data */
        	as8Buffer[s8BytesRecv] = '\0';

        	/* Convert client's message to integer */
        	int reqLen = atoi(as8Buffer);
		if(reqLen <= 0){
			close(ClntFd);
			continue; /* Bad request; skip it */
		}	
		reqLen = factNum(reqLen);

        	/* Send response to client with current sequence number */
        	sprintf(as8SeqNum, "%d\n", reqLen);
		printf("Response sent to client\n");
        	if(send(ClntFd, as8SeqNum, strlen(as8SeqNum),0) != strlen(as8SeqNum))
			fprintf(stderr, "Error on write");

    	}

    	/* Close server socket */
	 close(ClntFd);

    	return 0;
}
int factNum(int Num){
	if(Num == 0)
        	return 1;
    	else
        	return Num* factNum(Num - 1);
}

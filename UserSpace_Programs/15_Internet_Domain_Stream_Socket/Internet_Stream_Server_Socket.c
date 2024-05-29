/**************************************************************************
 * File:        Internet_Stream_Server_Socket.c
 *
 * Description: This program demonstrates a simple Internet domain stream socket server using the socket API. 
 * 		It creates a server socket, binds it to specified address, listens for incoming connections,
 *              accepts client connections, receives messages from clients, response back to clients.
 *
 * Usage:       ./Internet_Stream_Server_Socket
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        01/03/2024
 *
 * Reference:   "The Linux Programming Interface" book.
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
int factNum(int Num);
int main(void){
	printf("welcome to server application program In Intenet Domain stream socket\n");
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
		printf("Server socket created\n");

        	/* Set socket option to reuse address */
        	if(setsockopt(SrvrFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1){
            		perror("setsockopt error");
			printf("setsockopt error number: %d\n", errno);
            		continue;
       	 	}

        	/* Bind socket to address */
        	if(bind(SrvrFd, p->ai_addr, p->ai_addrlen) == 0){
			/* Success */
			printf("Server socket Binded with address \n");
            		break;
        	}
		close(SrvrFd);
	}

    	/* Check if binding was successful */
    	if(p == NULL){
        	fprintf(stderr, "Failed to bind\n");
		exit(EXIT_FAILURE);
    	}
	/* Listen for incoming connections */
    	if(listen(SrvrFd, 50) == -1){
		printf("listen error number: %d\n", errno); 
		perror("listen fail");
		exit(EXIT_FAILURE);
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
        		exit(EXIT_FAILURE);
        	}

		/* Print client's address */
		if(getnameinfo((struct sockaddr *) &strClntAddr, AddrLen,as8Host,NI_MAXHOST, as8Service, NI_MAXSERV, 0) == 0)
			snprintf(as8Addr, ADDRSTRLEN, "(%s, %s)", as8Host, as8Service);
		else
			snprintf(as8Addr, ADDRSTRLEN, "(?UNKNOWN?)");

		printf("Connection Established from %s client\n", as8Addr);
		/* Read client request, send sequence number back */
        	while(recv(ClntFd, as8Buffer, BUFFER_SIZE - 1, 0) > 0){
			/* Null-terminate received data */

			as8Buffer[BUFFER_SIZE-1]='\0';
			printf("\nData received from client: %s\n",as8Buffer);

        		/* Respond to client */
        		const char *response = "Message received!";
        		if(send(ClntFd, response, strlen(response),0) == -1){
				fprintf(stderr, "Error on write");
				break;
			}
			/* Send response to client */
                	printf("Response sent to client\n");

    		}

    		/* Close server socket */
	 	close(ClntFd);
	}
	 close(SrvrFd);

    	return 0;
}

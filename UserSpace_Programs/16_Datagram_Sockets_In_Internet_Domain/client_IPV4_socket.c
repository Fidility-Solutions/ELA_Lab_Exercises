/**************************************************************************
 * File:        client_IPV4_socket.c
 *
 * Description: This program demonstrates a simple Internet domain Datagram socket client
 *              using the socket API.It takes data from user to send to server from client,and 
 *              receives response from server and if user enter exit it will close the client socket.
 *
 * Usage:       ./client_IPV4_socket.c
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
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024


void errExit(const char *as8RcvMsg) {
    perror(as8RcvMsg);
    exit(EXIT_FAILURE);
}

/* Function: main
 *
 * Description: Entry point of the server program. Creates a Internet domain datagram socket server,
 *              takes input from user and send it to server, also receives response from server.
 *              and closes the client socket if user input is "exit".
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main(void) {
	printf("Welcome to the server-client application program using datagram sockets in the internet domain\n");
	/* Variable Declaration */
    	int s8SktFd;
    	struct sockaddr_in strSrvrAddr;
    	char as8RcvMsg[MAX_BUFFER_SIZE];
    	char as8Buffer[MAX_BUFFER_SIZE];

    	/* Create socket */
	printf("Client socket is created \n");
    	if ((s8SktFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	perror("Socket creation error");
        	exit(EXIT_FAILURE);
    	}
	/* clearing the structure before use */
    	memset(&strSrvrAddr, 0, sizeof(strSrvrAddr));

    	/* Set server address */
    	strSrvrAddr.sin_family = AF_INET;
    	strSrvrAddr.sin_port = htons(PORT);

    	/* Convert IPv4 address from text to binary form */
    	if (inet_pton(AF_INET, "127.0.0.1", &strSrvrAddr.sin_addr) <= 0) {
        	perror("inet_pton error");
        	exit(EXIT_FAILURE);
    	}

    	while (1) {
        	/* Take input from user */
        	printf("Enter as8RcvMsg to send to server (or type 'exit' to quit): ");
        	if(fgets(as8RcvMsg, MAX_BUFFER_SIZE, stdin)){

        		/* Send as8RcvMsg to server */
       			if (sendto(s8SktFd, as8RcvMsg, strlen(as8RcvMsg), 0, (struct sockaddr *)&strSrvrAddr, sizeof(strSrvrAddr))==-1){
				perror("Sendto server fail \n");
				exit(EXIT_FAILURE);
			}

			/* Check if user wants to exit */
                	if (strncmp(as8RcvMsg, "exit",4) == 0) {
                        	printf("User wants to exit\nExiting...\n");
                       	 	break;
                	}
		}

        	/* Receive response from server */
        	ssize_t RecvLen = recvfrom(s8SktFd,as8Buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        	if (RecvLen < 0) {
            		perror("Receive failed");
            		exit(EXIT_FAILURE);
        	}

        	/* Print response from server along with server's address */
        	as8Buffer[RecvLen] = '\0';
        	printf("Received from server at %s:%d: %s\n", inet_ntoa(strSrvrAddr.sin_addr), ntohs(strSrvrAddr.sin_port), as8Buffer);
    	}
	/*Close client */
    	close(s8SktFd);
    	return 0;
}



































































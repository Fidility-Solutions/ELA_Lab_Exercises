/**************************************************************************
 * File:        Internet_Datagram_IPV4_ClntSkt.c
 *
 * Description: This program demonstrates a simple Internet domain Datagram socket client
 *              using the socket API.It takes data from user to send to server from client,and 
 *              receives response from server and if user enter exit it will close the client socket.
 *
 * Usage:       ./Internet_Datagarm_IPV4_ClntSkt.c
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        02/03/2024
 *
 * Reference:   "The Linux Programming Interface" book.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024


void errExit(const char *as8RcvMsg){
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
	printf("\nWelcome to the server-client application program using datagram sockets in the internet domain\n");
	/* Variable Declaration */
    	int SktFd;
    	struct sockaddr_in strSrvrAddr;
    	char as8RcvMsg[MAX_BUFFER_SIZE];
    	char as8Buffer[MAX_BUFFER_SIZE];

    	/* Create socket */
	printf("Client socket is created \n");
    	if ((SktFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	perror("Socket creation error");
        	exit(EXIT_FAILURE);
    	}
	/* clearing the structure before use */
    	memset(&strSrvrAddr, 0, sizeof(strSrvrAddr));

    	/* Set server address */
    	strSrvrAddr.sin_family = AF_INET;
    	strSrvrAddr.sin_port = htons(PORT);
	strSrvrAddr.sin_addr.s_addr= INADDR_ANY;

    	/* Convert IPv4 address from text to binary form */
//    	if(inet_pton(AF_INET, "127.0.0.1", &strSrvrAddr.sin_addr) == 0)
  //      	errExit("inet_pton error");

    	while(1){
        	/* Take input from user */
        	printf("Enter data to send to server (or type 'exit' to quit): ");
        	if(fgets(as8RcvMsg, MAX_BUFFER_SIZE, stdin)){
			 as8RcvMsg[strcspn(as8RcvMsg, "\n")] = '\0';

        		/* Send as8RcvMsg to server */
       			if(sendto(SktFd, as8RcvMsg, strlen(as8RcvMsg), MSG_CONFIRM, (struct sockaddr *)&strSrvrAddr, sizeof(strSrvrAddr))==-1)
				errExit("Sendto server fail \n");

			/* Check if user wants to exit */
                	if(strncmp(as8RcvMsg, "exit",4) == 0){
                        	printf("User wants to exit\nExiting...\n");
                       	 	break;
                	}
		}
        	/* Receive response from server */
        	ssize_t RecvLen = recvfrom(SktFd,as8Buffer, MAX_BUFFER_SIZE, MSG_WAITALL,NULL,NULL);
        	if(RecvLen == -1)
            		errExit("Receive failed");

        	/* Print response from server along with server's address */
        	as8Buffer[RecvLen] = '\0';
        	printf("Received from server at %s:%d:\"%s\"\n", inet_ntoa(strSrvrAddr.sin_addr), ntohs(strSrvrAddr.sin_port), as8Buffer);
    	}
	/*Close client */
    	close(SktFd);
    	return 0;
}


/**************************************************************************
 * File:        Internet_Datagram_IPV4_SrvrSkt.c
 *
 * Description:	This program demonstrates a simple Internet domain Datagram socket server using the socket API. 
 * 		It creates a server socket, binds it to an Internet domain socket path, receives messages 
 * 		from clients, converts them to uppercase, sends them back to the client, and finally closes 
 * 		the server socket.
 *          
 * Usage:       ./Internet_Datagram_IPV4_SrvtSkt.c
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
#include<ctype.h>
#define PORT 8080
#define MAX_BUFFER_SIZE 1024

void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

/* Function: main
 *
 * Description:	Entry point of the server program. It creates an Internet domain socket server, 
 * 		receives client messages, sends responses to clients, and if the server needs 
 * 		any information from a client, it requests send to the client. Finally, it closes the server socket
 *             
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */


int main(void){
	printf("\nWelcome to the server-client application program using datagram sockets in the internet domain\n");
	/*Variable Declaration */
    	int SrvrFd;
    	struct sockaddr_in strSrvrAddr, strClntAdrr;
    	socklen_t ClntAddrLen = sizeof(strClntAdrr);
   	char s8ClntBuff[MAX_BUFFER_SIZE];
	char s8SrvrBuff[MAX_BUFFER_SIZE];

    	/* Create socket */
	printf("Server socket is created \n");
    	if((SrvrFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
        	perror("Socket creation error");
        	exit(EXIT_FAILURE);
    	}
	/* clear the structure before proceed */
    	memset(&strSrvrAddr, 0, sizeof(strSrvrAddr));
    	memset(&strClntAdrr, 0, sizeof(strClntAdrr));

    	/* Load the server address */
	/* AF_INET is for IPV4 */
    	strSrvrAddr.sin_family = AF_INET;
	/* Any type of IPV4 address */
    	strSrvrAddr.sin_addr.s_addr = INADDR_ANY;
	/* host to n/w byte order */
    	strSrvrAddr.sin_port = htons(PORT);

    	/* Bind socketwith specified address */
    	if(bind(SrvrFd, (struct sockaddr *)&strSrvrAddr, sizeof(strSrvrAddr)) < 0)
        	errExit("Bind failed");
    	printf("Bind successful with specified server address...\n");
	/* Extract server IP address */
    	char server_ip[INET_ADDRSTRLEN];
   	inet_ntop(AF_INET, &strSrvrAddr.sin_addr, server_ip, INET_ADDRSTRLEN);
	printf("UDP Server is ready to receive data from clients...\n");
    	while(1){
        	/* Receive message from client */
        	ssize_t RecvLen = recvfrom(SrvrFd, s8ClntBuff, MAX_BUFFER_SIZE,0, (struct sockaddr *)&strClntAdrr, &ClntAddrLen);
        	if (RecvLen < 0) 
            		errExit("Receive failed");

        	s8ClntBuff[RecvLen] = '\0';
        	/* Print client's message */
        	printf("\nReceived from client at %s:%d is:\"%s\" \n", inet_ntoa(strClntAdrr.sin_addr), ntohs(strClntAdrr.sin_port), s8ClntBuff);
		if(strncmp(s8ClntBuff,"exit\n",4)==0){
			printf("\nClient %s disconnected\n",inet_ntoa(strClntAdrr.sin_addr));
			continue;
		}
		else{
        		/* Change message to uppercase */
        		for (int i = 0;s8ClntBuff[i] != '\0'; ++i) {
            			s8ClntBuff[i] = toupper(s8ClntBuff[i]);
        		}

        		/* Send uppercase message back to client */
        		if(sendto(SrvrFd,s8ClntBuff, strlen(s8ClntBuff), MSG_CONFIRM, (struct sockaddr *)&strClntAdrr, ClntAddrLen)!=RecvLen){
				perror("sendto:unable to send\n");
				continue;
			}
			printf("Sent response to client with Uppercase\n");
			
		}
	}

	/* Close the server */
    	close(SrvrFd);

    	return 0;
}



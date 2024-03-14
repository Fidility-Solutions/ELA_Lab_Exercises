/**************************************************************************
 * File:        server_IPV4_socket.c
 *
 * Description:	This program demonstrates a simple Internet domain Datagram socket server using the socket API. 
 * 		It creates a server socket, binds it to an Internet domain socket path, receives messages 
 * 		from clients, converts them to uppercase, sends them back to the client, and finally closes 
 * 		the server socket.
 *          
 * Usage:       ./server_IPV4_socket.c
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        01/03/2024
 *
 * Reference:   The Linux Programming Interface book.
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


int main(void) {
	printf("Welcome to the server-client application program using datagram sockets in the internet domain\n");
	/*Variable Declaration */
    	int s8SrvrFd;
    	struct sockaddr_in strSrvrAddr, strClntAdrr;
    	socklen_t ClntAddrLen = sizeof(strClntAdrr);
   	char s8ClntBuff[MAX_BUFFER_SIZE];
	char s8SrvrBuff[MAX_BUFFER_SIZE];

    	/* Create socket */
	printf("The server socket is created \n");
    	if ((s8SrvrFd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	perror("Socket creation error");
        	exit(EXIT_FAILURE);
    	}
	/* clear the structure before proceed */
	printf("Clearing the structure before proceeding the communication...\n");
    	memset(&strSrvrAddr, 0, sizeof(strSrvrAddr));
    	memset(&strClntAdrr, 0, sizeof(strClntAdrr));

    	/* Load the server address */
	/* AF_INET is for IPV4 */
	printf("Loaded the sever family, address and port number to server socket \n");
    	strSrvrAddr.sin_family = AF_INET;
	/* Any type of IPV4 address */
    	strSrvrAddr.sin_addr.s_addr = INADDR_ANY;
	/* host to n/w byte order */
    	strSrvrAddr.sin_port = htons(PORT);

    	/* Bind socketwith specified address */
	printf("Binding the server socket with specified server address...\n");
    	if (bind(s8SrvrFd, (struct sockaddr *)&strSrvrAddr, sizeof(strSrvrAddr)) < 0) {
        	perror("Bind failed");
        	exit(EXIT_FAILURE);

    	}
	// Extract server IP address
    	char server_ip[INET_ADDRSTRLEN];
   	inet_ntop(AF_INET, &strSrvrAddr.sin_addr, server_ip, INET_ADDRSTRLEN);
    	printf("Server IP address: %s\n", server_ip);
	printf("Server Running...\n");
	printf("Server is ready to take data from clients\n");
    	while (1) {
        	/* Receive message from client */
        	ssize_t RecvLen = recvfrom(s8SrvrFd, s8ClntBuff, MAX_BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&strClntAdrr, &ClntAddrLen);
        	if (RecvLen < 0) {
            		perror("Receive failed");
            		exit(EXIT_FAILURE);
        	}

        	s8ClntBuff[RecvLen] = '\0';
        	/* Print client's message */
        	printf("Received from client at %s:%d: %s\n", inet_ntoa(strClntAdrr.sin_addr), ntohs(strClntAdrr.sin_port), s8ClntBuff);
		if(strncmp(s8ClntBuff,"exit\n",4)==0){
			printf("Client %s disconnected\n",inet_ntoa(strClntAdrr.sin_addr));
			continue;
		}
		else{
        		/* Change message to uppercase */
        		for (int i = 0;s8ClntBuff[i] != '\0'; ++i) {
            			s8ClntBuff[i] = toupper(s8ClntBuff[i]);
        		}
        		/* Send uppercase message back to client */
        		if(sendto(s8SrvrFd,s8ClntBuff, strlen(s8ClntBuff), MSG_CONFIRM, (struct sockaddr *)&strClntAdrr, ClntAddrLen)!=RecvLen){
				perror("sendto:unable to send\n");
				continue;
			}
			
		}
	}

	/* Close the server */
    	close(s8SrvrFd);

    	return 0;
}



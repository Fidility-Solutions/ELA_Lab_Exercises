/******************************************************************************
 * File:        Unix_Datagram_ClientSocket.c
 *
 * Description: This program demonstrates a simple Unix domain Datagram socket client using the socket API. 
 * 		It creates a client socket and binds it to a specified address. It takes user inputs and sends 
 * 		them to the server, and also receives data from the server.
 *             
 * Usage:       ./Unix_Datagram_ClientSocket
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        01/03/2024
 *
 * Reference:   The "Linux Programming Interface" book.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/UNIX_Datagram_Socket"
#define BUF_SIZE 256

/*
 * function:    errExit
 *
 * Description: This function will notify the error which is cased by the program and exit from the program 
 *
 * parameter:   constant char types pointer variable.
 *
 * Return:      None
 *
 */

void errExit(const char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

/* Function: main
 *
 * Description: Entry point of the server program. Creates a Unix domain Datagram socket client,              ,
 *              and perform commucation with server.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */


int main(){
	printf("Welcome to client application that uses stream sockets in UNIX domain \n");

	/* variable Declaration */
    	int SrvrSktFd;

    	struct sockaddr_un strSrvrAddr, strClntAddr;
    	socklen_t strSrvrAddr_len = sizeof(struct sockaddr_un);
	/*ClntBuffer is used to take data from user and send it to server */
    	char ClntBuffer[BUF_SIZE];

	/* RecvSrvrBuffer is used to capture the data from server */
    	char RecvSrvrBuffer[BUF_SIZE];
    	ssize_t BytesRecv;

    	/* Create UNIX Domain Datagram client socket */
    	SrvrSktFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    	if (SrvrSktFd == -1) 
        	errExit("socket creation fail");
    	printf("Client socket created\n");

    	/* Set up client address */
    	memset(&strClntAddr, 0, sizeof(struct sockaddr_un));
    	strClntAddr.sun_family = AF_UNIX;
   	snprintf(strClntAddr.sun_path, sizeof(strClntAddr.sun_path),"%s.%ld",SOCKET_PATH ,(long) getpid());

    	/* Bind the client socket with specified adress */
    	if(bind(SrvrSktFd, (struct sockaddr *) &strClntAddr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind error");
	printf("Bind successful with specified address\n");

    	/* Construct address of server */
	memset(&strSrvrAddr, 0, sizeof(struct sockaddr_un));
	strSrvrAddr.sun_family = AF_UNIX;
	strncpy(strSrvrAddr.sun_path, SOCKET_PATH, sizeof(strSrvrAddr.sun_path) - 1);

    	while(1){
        	printf("Enter message to send (type 'exit' to quit): ");
        	fgets(ClntBuffer, sizeof(ClntBuffer), stdin);


        	/* Send message to server */
        	if(sendto(SrvrSktFd, ClntBuffer, strlen(ClntBuffer), 0,(struct sockaddr *)&strSrvrAddr, strSrvrAddr_len) != strlen(ClntBuffer)) 
           		errExit("sendto server fail");

		/* Check if the user wants to exit */
                if(strncmp(ClntBuffer, "exit", 4) == 0){
                        printf("Exiting...\n");
                        break;
                }
		 printf("Message sent successfully to server.\n");

		/* Receive messages from server */
		BytesRecv=recvfrom(SrvrSktFd,RecvSrvrBuffer,BUF_SIZE,0,NULL,NULL);
		if(BytesRecv==-1)
			errExit("receive fail.");

		printf("Response received from server: %s \n",  RecvSrvrBuffer);

    	}

    	remove(strClntAddr.sun_path); /* Remove client socket pathname */

    	/* Close client socket */
	close(SrvrSktFd);

    return 0;
}


/**************************************************************************
 * File:        UNIX_Datagram_Server_Socket.c
 *
 * Description: This program demonstrates a simple Unix domain Datagram socket server
 *              using the socket API. It creates a server socket, binds it to
 *              a Unix domain socket path, listens for incoming connections,
 *              accepts client connections, receives messages from clients,
 *              echoes the messages back to clients, and finally closes the
 *              server socket.
 *
 * Usage:       ./UNIX_Datagram_Server_Socket.c
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
#include <ctype.h>
#include <errno.h>

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
 * Description: Entry point of the server program. Creates a Unix domain socket server,
 *              accepts client connections, receives and echoes messages from/to clients,
 *              and closes the server socket.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main(){
	printf("Welcome to client-server application that uses Datagram sockets in UNIX domain \n");

	/*variable declaration */
    	int SrvrSktFd;
	/* declaration of strcture to represent address of sockets */
    	struct sockaddr_un strSrvrAdrr, strClntAddr;
	/*length of socket */
    	socklen_t ClntAddrLen = sizeof(struct sockaddr_un);
    	char RecvBuffer[BUF_SIZE];

    	/* Create A UNIX Domain Datagram server socket */
    	SrvrSktFd = socket(AF_UNIX, SOCK_DGRAM, 0);
	printf("Server socket created using socket () sys call ...\n");
    	if(SrvrSktFd == -1) 
        	errExit("socket creation failed");

    	/* remove the socket if already present */
    	if(remove(SOCKET_PATH) == -1 && errno != ENOENT)
		fprintf(stdout, "remove-%s", SOCKET_PATH);

	/* Clearing the structure before use */
	memset(&strSrvrAdrr, 0, sizeof(strSrvrAdrr));

    	/* Set up server address with socket family and path to the socket file in the file system */
    	strSrvrAdrr.sun_family = AF_UNIX;
    	strncpy(strSrvrAdrr.sun_path, SOCKET_PATH, sizeof(strSrvrAdrr.sun_path) - 1);

    	/* Bind server socket to address */
	printf("Binding the server socket to its well known address ...\n");
    	if(bind(SrvrSktFd, (struct sockaddr *)&strSrvrAdrr,sizeof(struct sockaddr_un)) == -1) 
        	errExit("binding error");

    	printf("Server is running...\n");

    	while(1){
        	/* Receive message from client */
        	ssize_t BytesRecv = recvfrom(SrvrSktFd, RecvBuffer,BUF_SIZE, 0,(struct sockaddr *)&strClntAddr, &ClntAddrLen);
        	if(BytesRecv == -1) 
            		errExit("recvfrom client error");
        	
        	/* Null-terminate received data to print as string */
        	RecvBuffer[BytesRecv] = '\0';

       		 printf("Server received data from client: %s\n", RecvBuffer);
		printf("Server received %ld bytes from %s\n", (long) BytesRecv,strClntAddr.sun_path);

        	/* Check if the client wants to exit */
        	if(strncmp(RecvBuffer, "exit", 4) == 0){
            		printf("Client %s exited.\n",strClntAddr.sun_path);
            		continue;
        	}
		/* Processing Received data: Converting the message to uppercase */
		for(int j = 0; j < BytesRecv; j++)
			RecvBuffer[j] = toupper((unsigned char) RecvBuffer[j]);

        	/* Send message back to client */
        	if(sendto(SrvrSktFd, RecvBuffer, BytesRecv, 0,(struct sockaddr *)&strClntAddr, ClntAddrLen)!=BytesRecv)             		errExit("sendto");
        	
	 	/*If BytesRecv from client is zero :Client closed the connection */
                if(BytesRecv == 0) 
                        printf("Client disconnected.\n");

    }

    /* Close server socket */
    close(SrvrSktFd);

    /* Remove socket file */
    unlink(SOCKET_PATH);

    return 0;
}


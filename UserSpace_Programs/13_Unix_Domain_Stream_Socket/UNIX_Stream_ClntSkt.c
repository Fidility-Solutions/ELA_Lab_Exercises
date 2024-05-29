/******************************************************************************
 * File		: UNIX_Stream_ClntSkt.c
 *
 * Description	: This program demonstrates a simple Unix domain socket client using the socket API. 
 * 		  It creates a client socket and binds it to a specified address. 
 * 		  It then establishes a connection with the server, reads user input, 
 * 		  sends the data to the server, and also receives data from the server.
 *             
 * Usage	: ./UNIX_Stream_ClntSkt.c
 *
 * Author	: Fidility Solutions.
 *  
 * Date		: 01/03/2024
 *
 * Reference	: "The Linux Programming Interface" book.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include<errno.h>

#define SOCKET_PATH "/tmp/UNIX_Stream_Socket"
#define BUF_SIZE 256



void errExit(const char *message){
    perror(message);
    exit(EXIT_FAILURE);
}

/* Function: main
 *
 * Description: Entry point of the server program. Creates a Unix domain socket client,              ,
 *              and perform commucation with server.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main(void){
	 printf("\nWelcome to client-server application that uses stream sockets in UNIX domain \n");

	/* variable Declaration */
    	int ClntSktFd;
	/* declaration of strcture to represent server address */
    	struct sockaddr_un strSrvrAddr,strClntAddr;
    	char s8Buffer[256];

    	/* Create UNIX Domain stream client socket */
	printf("Client socket created using socket() sys call ...\n");
    	ClntSktFd = socket(AF_UNIX, SOCK_STREAM, 0);
    	if(ClntSktFd == -1) 
        	errExit("socket creation failed");
	strClntAddr.sun_family = AF_UNIX;
        snprintf(strClntAddr.sun_path, sizeof(strClntAddr.sun_path),"%s.%ld",SOCKET_PATH ,(long) getpid());
	if (bind(ClntSktFd, (const struct sockaddr *)&strClntAddr, sizeof(strClntAddr)) < 0) {
        	perror("bind failed");
       		 close(ClntSktFd);
        	exit(EXIT_FAILURE);
    	}
    	/* Set up server address */
    	memset(&strSrvrAddr, 0, sizeof(strSrvrAddr));
    	strSrvrAddr.sun_family = AF_UNIX;
    	strncpy(strSrvrAddr.sun_path, SOCKET_PATH, sizeof(strSrvrAddr.sun_path) - 1);

    	/* Establish connection with server */
	printf("Trying to connect with server...\n");
    	if(connect(ClntSktFd, (struct sockaddr *)&strSrvrAddr, sizeof(strSrvrAddr)) == -1) 
        	errExit("connection failed");
	/* Print connection message */
    	printf("Connected to server.\n");

    	while(1){
        /* Read message from user */
	printf("Please enter message to send (type 'exit' to quit):");
        fgets(s8Buffer, sizeof(s8Buffer), stdin);

        /* Send message to server */
        if(send(ClntSktFd, s8Buffer, strlen(s8Buffer), 0) == -1) 
            	errExit("send");

	/* if user message is 'exit' then exit */
        if(strncmp(s8Buffer, "exit\n",4) == 0)
                  break;

        /* Receive message from server */
        ssize_t BytesRecv = recv(ClntSktFd, s8Buffer, BUF_SIZE, 0);

	/* close the connection if no bytes received */
        if(BytesRecv <= 0){
            	/* Server closed the connection */
            	printf("Server closed the connection.\n");
            	break;
        }
        
        /* Null-terminate received data to print as string */
        s8Buffer[BytesRecv] = '\0';
        printf("Received from server: %s\n", s8Buffer);
    }

    	/* Close client socket */
    	close(ClntSktFd);

    	return 0;
}


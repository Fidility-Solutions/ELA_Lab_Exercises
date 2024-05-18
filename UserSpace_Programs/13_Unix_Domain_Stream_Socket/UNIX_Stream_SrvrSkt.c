/******************************************************************************
 * File:        UNIX_Stream_SrvrSkt.c
 *
 * Description: This program demonstrates a simple Unix domain socket server
 *              using the socket API. It creates a server socket, binds it to
 *              a Unix domain socket path, listens for incoming connections,
 *              accepts client connections, receives messages from clients,
 *              echoes the messages back to clients, and finally closes the
 *              server socket.
 *
 * Usage:	./UNIX_Stream_SrvrSkt
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        01/03/2024
 *
 * Reference:   "The Linux Programming Interface" book.
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <errno.h>
#include <arpa/inet.h> 
#define SOCKET_PATH "/tmp/UNIX_Stream_Socket"
#define MAX_CLIENTS 10
#define BUF_SIZE 256

void errExit(const char *message) {
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
int main(void){
	printf("Welcome to client-server application that uses stream sockets in UNIX domain \n");

	/* variable declaration */
    	int SrvrSktFd, ClntSktFds[MAX_CLIENTS], NumofClnts = 0;

	/* declaration of strcture to represent address of sockets */
    	struct sockaddr_un strSrvrAddr, strClntAddr;

	/*length of socket */
    	socklen_t ClntAddrLen = sizeof(strClntAddr);
    	char s8buffer[BUF_SIZE];
	char s8UserData[BUF_SIZE];

	/*remove socket if already present */
	if(remove(SOCKET_PATH) == -1 && errno != ENOENT)
                fprintf(stdout, "remove-%s", SOCKET_PATH);
	
	/* Construct server socket address, bind socket to it, and make this a listening socket */

    	/* Create A UNIX Domain stream server socket */
	printf("Server socket created using socket() sys call ...\n");
    	SrvrSktFd = socket(AF_UNIX, SOCK_STREAM, 0);
    	if(SrvrSktFd == -1) 
        	errExit("socket creation fail");

	/* clearing the structure before use */
    	memset(&strSrvrAddr, 0, sizeof(strSrvrAddr));
   	/* Set up server address with socket family and path to the socket file in the file system */
    	strSrvrAddr.sun_family = AF_UNIX;
    	strncpy(strSrvrAddr.sun_path, SOCKET_PATH, sizeof(strSrvrAddr.sun_path) - 1);

    	/* Bind server socket to address */
    	if(bind(SrvrSktFd, (struct sockaddr *)&strSrvrAddr, sizeof(strSrvrAddr)) == -1) 
        	errExit("bind error");
	printf("The server socket binds to its predetermined or specified address\n");

    	/* Listen incoming connection from other socket to connect */
	printf("The Server socket is ready to listen from client sockets ..\n");
    	if(listen(SrvrSktFd, MAX_CLIENTS) == -1) 
        	errExit("listen fail");

    	printf("Server is running...\n");

    	while(1){
        	/* Accept incoming connection from other sockets*/
		printf("The Server socket is ready to accept incoming connection from other sockets...\n");
        	int ClntSktFd = accept(SrvrSktFd, (struct sockaddr *)&strClntAddr, &ClntAddrLen);
		// Extract client's IP address and port number
		printf("Client connected: %s\n",strClntAddr.sun_path);
        	if(ClntSktFd == -1) 
           		errExit("accept");
        	//printf("Client connected:%d\n", ClntSktFd);
		printf("Enter exit to 'quit' from serever\n");

        	/* Receive and echo messages from/to the client */
        	ssize_t BytesRecv;
        	while((BytesRecv = recv(ClntSktFd, s8buffer, sizeof(s8buffer), 0)) > 0){

            		/* Null-terminate received data to print as string */
            		s8buffer[BytesRecv] = '\0';
            		printf("The Data received from client: %s\n", s8buffer);
			if(strncmp(s8buffer,"exit\n",4)==0){
//					printf("Client %d Disconneted\n",ClntSktFd);
					printf("Client %s Disconnected\n",strClntAddr.sun_path);
					continue;
			}
			/* Processing Received data: Converting the message to uppercase */
    			for(int i = 0; i < strlen(s8buffer); i++) 
        			s8buffer[i] = toupper(s8buffer[i]);
    			

    			/* Null-terminate the string */
    			s8buffer[BytesRecv] = '\0';
			/* Echo back to client with uppercase letters to know the server received correct Data */
            		if(send(ClntSktFd, s8buffer, BytesRecv, 0) == -1) 
                		errExit("send fail");
			printf("The input from the client is changed to Upper Case\n");
			printf("Message sent back to client:%s\n",s8buffer);

        	}

        	/* If client Disconnected Close the client socket */ 
        	close(ClntSktFd);
    	}

    	/* Close server socket */
    	close(SrvrSktFd);

    	/* Remove socket file */
    	unlink(SOCKET_PATH);

    	return 0;
}


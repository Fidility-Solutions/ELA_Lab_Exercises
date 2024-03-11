/******************************************************************************
 * File:        Dclient_socket.c
 *
 * Description: This program demonstrates a simple Unix domain Datagram socket client using the socket API. 
 * 		It creates a client socket and binds it to a specified address. It takes user inputs and sends 
 * 		them to the server, and also receives data from the server.
 *             
 * Usage:       ./Dclient_socket.c
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
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define SOCKET_PATH "/tmp/UNIX_Datagram_Socket"
#define BUF_SIZE 256

void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void usageErr(const char *programName, const char *message) {
    fprintf(stderr, "Usage: %s %s\n", programName, message);
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


int main() {
	printf("Welcome to client-server application that uses stream sockets in UNIX domain \n");

	/* variable Declaration */
    	int s8SrvrSktFd;

    	struct sockaddr_un strSrvrAddr, strClntAddr;
    	socklen_t strSrvrAddr_len = sizeof(struct sockaddr_un);
	/*s8buffer is used to take data from user and send it to server */
    	char s8buffer[BUF_SIZE];

	/* storage is used to capture the data from server */
    	char storage[BUF_SIZE];
    	ssize_t BytesRecv;

    	/* Create UNIX Domain Datagram client socket */
    	printf("Client socket created using socket () sys call ...\n");
    	s8SrvrSktFd = socket(AF_UNIX, SOCK_DGRAM, 0);
    	if (s8SrvrSktFd == -1) 
        	errExit("socket creation fail");
    	

    	/* Set up client address */
    	memset(&strClntAddr, 0, sizeof(struct sockaddr_un));
    	strClntAddr.sun_family = AF_UNIX;
   	snprintf(strClntAddr.sun_path, sizeof(strClntAddr.sun_path),"%s.%ld",SOCKET_PATH ,(long) getpid());

    	/* Bind the client socket with specified adress */
    	if (bind(s8SrvrSktFd, (struct sockaddr *) &strClntAddr, sizeof(struct sockaddr_un)) == -1)
		errExit("bind error");

    	/* Construct address of server */
	memset(&strSrvrAddr, 0, sizeof(struct sockaddr_un));
	strSrvrAddr.sun_family = AF_UNIX;
	strncpy(strSrvrAddr.sun_path, SOCKET_PATH, sizeof(strSrvrAddr.sun_path) - 1);

    	while (1) {
        	printf("Enter message to send (type 'exit' to quit): ");
        	fgets(s8buffer, sizeof(s8buffer), stdin);


        	/* Send message to server */
        	if (sendto(s8SrvrSktFd, s8buffer, strlen(s8buffer), 0,(struct sockaddr *)&strSrvrAddr, strSrvrAddr_len) != strlen(s8buffer)) 
           		errExit("sendto server fail");

		/* Check if the user wants to exit */
                if (strncmp(s8buffer, "exit", 4) == 0) {
                        printf("Exiting...\n");
                        break;
                }
		 printf("Message sent successfully.\n");

		/* Receive messages from server */
		BytesRecv=recvfrom(s8SrvrSktFd,storage,BUF_SIZE,0,NULL,NULL);
		if(BytesRecv==-1)
			errExit("receive fail.");

		printf("Response from server  %.*s\n", (int) BytesRecv, storage);

    	}
    	remove(strClntAddr.sun_path); /* Remove client socket pathname */
	exit(EXIT_SUCCESS);

    	/* Close client socket */
	close(s8SrvrSktFd);

    return 0;
}


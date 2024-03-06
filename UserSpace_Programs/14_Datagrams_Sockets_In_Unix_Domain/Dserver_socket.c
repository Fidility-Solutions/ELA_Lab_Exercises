/**************************************************************************
 * File:        Dserver_socket.c
 *
 * Description: This program demonstrates a simple Unix domain Datagram socket server
 *              using the socket API. It creates a server socket, binds it to
 *              a Unix domain socket path, listens for incoming connections,
 *              accepts client connections, receives messages from clients,
 *              echoes the messages back to clients, and finally closes the
 *              server socket.
 *
 * Usage:       ./server_socket.c
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
#include <ctype.h>
#include <errno.h>

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
 * Description: Entry point of the server program. Creates a Unix domain socket server,
 *              accepts client connections, receives and echoes messages from/to clients,
 *              and closes the server socket.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main() {
	printf("Welcome to client-server application that uses Datagram sockets in UNIX domain \n");

	/*variable declaration */
    	int s8SrvrSktFd;
	/* declaration of strcture to represent address of sockets */
    	struct sockaddr_un strSrvrAdrr, strClntAddr;
	/*length of socket */
    	socklen_t ClntAddrLen = sizeof(struct sockaddr_un);
    	char s8buffer[BUF_SIZE];

    	/* Create A UNIX Domain Datagram server socket */
    	s8SrvrSktFd = socket(AF_UNIX, SOCK_DGRAM, 0);
	printf("Server socket created using socket () sys call ...\n");
    	if (s8SrvrSktFd == -1) 
        	errExit("socket creation failed");

    	/* remove the socket if already present */
    	if (remove(SOCKET_PATH) == -1 && errno != ENOENT)
		fprintf(stdout, "remove-%s", SOCKET_PATH);

	/* clearing the structure before use */
	memset(&strSrvrAdrr, 0, sizeof(strSrvrAdrr));

    	/* Set up server address with socket family and path to the socket file in the file system */
    	strSrvrAdrr.sun_family = AF_UNIX;
    	strncpy(strSrvrAdrr.sun_path, SOCKET_PATH, sizeof(strSrvrAdrr.sun_path) - 1);

    	/* Bind server socket to address */
	printf("Binding the server socket to its well known address ...\n");
    	if (bind(s8SrvrSktFd, (struct sockaddr *)&strSrvrAdrr,sizeof(struct sockaddr_un)) == -1) 
        	errExit("binding error");

    	printf("Server is running...\n");

    	while (1) {
        	/* Receive message from client */
        	ssize_t BytesRecv = recvfrom(s8SrvrSktFd, s8buffer,BUF_SIZE, 0,(struct sockaddr *)&strClntAddr, &ClntAddrLen);
        	if (BytesRecv == -1) 
            		errExit("recvfrom client error");
        	
	// printf("Client connected:%d\n", client_socket);
	
        	/* Null-terminate received data to print as string */
        	s8buffer[BytesRecv] = '\0';

       		 printf("Server received data from client: %s\n", s8buffer);
		printf("Server received %ld bytes from %s\n", (long) BytesRecv,strClntAddr.sun_path);

        	/* Check if the client wants to exit */
        	if (strncmp(s8buffer, "exit", 4) == 0) {
            		printf("Client requested to exit. Exiting...\n");
            		break;
        	}
		/* Processing Received data: Converting the message to uppercase */
		for (int j = 0; j < BytesRecv; j++)
			s8buffer[j] = toupper((unsigned char) s8buffer[j]);

        	/* Send message back to client */
        	if(sendto(s8SrvrSktFd, s8buffer, BytesRecv, 0,(struct sockaddr *)&strClntAddr, ClntAddrLen)!=BytesRecv)             		errExit("sendto");
        	
	 	/*If BytesRecv from client is zero :Client closed the connection */
                if (BytesRecv == 0) 
                        printf("Client disconnected.\n");

    }

    /* Close server socket */
    close(s8SrvrSktFd);

    // Remove socket file
   // unlink(SOCKET_PATH);

    return 0;
}


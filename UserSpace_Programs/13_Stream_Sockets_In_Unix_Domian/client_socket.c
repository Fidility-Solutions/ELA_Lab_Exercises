/******************************************************************************
 * File:        client_socket.c
 *
 * Description: This program demonstrates a simple Unix domain socket client using the socket API. 
 * 		It creates a client socket and binds it to a specified address. 
 * 		It then establishes a connection with the server, reads user input, 
 * 		sends the data to the server, and also receives data from the server.
 *             
 * Usage:       ./client_socket.c
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
#include "tlpi_hdr.h"
#include<errno.h>

#define SOCKET_PATH "/tmp/stream_socket"
#define BUF_SIZE 256

/* Function: main
 *
 * Description: Entry point of the server program. Creates a Unix domain socket client,              ,
 *              and perform commucation with server.
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main() {
	 printf("Welcome to client-server application that uses stream sockets in UNIX domain \n");

	/* variable Declaration */
    	int client_socket_fd;
	/* declaration of strcture to represent server address */
    	struct sockaddr_un server_addr;
    	char buffer[256];

    	/* Create UNIX Domain stream client socket */
	printf("Client socket created using socket () sys call ...\n");
    	client_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    	if (client_socket_fd == -1) {
        	perror("socket");
        	exit(EXIT_FAILURE);
    	}

    	/* Set up server address */
    	memset(&server_addr, 0, sizeof(server_addr));
    	server_addr.sun_family = AF_UNIX;
    	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    	/* Establish connection with server */
	printf("Trying to connect with server...\n");
    	if (connect(client_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        	perror("connect");
        	exit(EXIT_FAILURE);
    	}
	/* Print connection message */
    	printf("Connected to server. Enter message to send (type 'exit' to quit):\n");

    	while (1) {
        /* Read message from user */
	printf("Please Enter data to pass to the server:");
        fgets(buffer, sizeof(buffer), stdin);
	/* if user message is 'exit' then exit */
        if (strcmp(buffer, "exit\n") == 0) {
          	  break;
        }

        /* Send message to server */
        if (send(client_socket_fd, buffer, strlen(buffer), 0) == -1) {
            	perror("send");
            	exit(EXIT_FAILURE);
        }

        /* Receive message from server */
        ssize_t bytes_received = recv(client_socket_fd, buffer, BUF_SIZE, 0);

	/* close the connection if no bytes received */
        if (bytes_received <= 0) {

            	/* Server closed the connection */
            	printf("Server closed the connection.\n");
            	break;
        }
        
        /* Null-terminate received data to print as string */
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);
    }

    	/* Close client socket */
    	close(client_socket_fd);

    	return 0;
}


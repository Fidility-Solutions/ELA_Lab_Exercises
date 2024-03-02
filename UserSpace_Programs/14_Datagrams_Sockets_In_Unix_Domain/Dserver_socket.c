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
#include "tlpi_hdr.h"
#define SOCKET_PATH "/tmp/datagram_socket"

#define BUF_SIZE 256
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
    	int server_socket_fd;
	/* declaration of strcture to represent address of sockets */
    	struct sockaddr_un server_addr, client_addr;
	/*length of socket */
    	socklen_t client_addr_len = sizeof(struct sockaddr_un);
    	char buffer[BUF_SIZE];

    	/* Create A UNIX Domain Datagram server socket */
    	server_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	printf("Server socket created using socket () sys call ...\n");
    	if (server_socket_fd == -1) {
        	perror("socket");
        	exit(EXIT_FAILURE);
    	}
    	/* remove the socket if already present */
    	if (remove(SOCKET_PATH) == -1 && errno != ENOENT)
		fprintf(stdout, "remove-%s", SOCKET_PATH);

	/* clearing the structure before use */
	memset(&server_addr, 0, sizeof(server_addr));

    	/* Set up server address with socket family and path to the socket file in the file system */
    	server_addr.sun_family = AF_UNIX;
    	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    	/* Bind server socket to address */
	printf("Binding the server socket to its well known address ...\n");
    	if (bind(server_socket_fd, (struct sockaddr *)&server_addr,sizeof(struct sockaddr_un)) == -1) {
        	perror("bind");
        	exit(EXIT_FAILURE);
    	}

    	printf("Server is running...\n");

    	while (1) {
        	/* Receive message from client */
        	ssize_t bytes_received = recvfrom(server_socket_fd, buffer,BUF_SIZE, 0,(struct sockaddr *)&client_addr, &client_addr_len);
        	if (bytes_received == -1) {
            	perror("recvfrom");
            	exit(EXIT_FAILURE);
        	}
	// printf("Client connected:%d\n", client_socket);
	
        	/* Null-terminate received data to print as string */
        	buffer[bytes_received] = '\0';

       		 printf("Server received data from client: %s\n", buffer);
		printf("Server received %ld bytes from %s\n", (long) bytes_received,client_addr.sun_path);

        	/* Check if the client wants to exit */
        	if (strncmp(buffer, "exit", 4) == 0) {
            		printf("Client requested to exit. Exiting...\n");
            		break;
        	}
		/* Processing Received data: Converting the message to uppercase */
		for (int j = 0; j < bytes_received; j++)
			buffer[j] = toupper((unsigned char) buffer[j]);

        	/* Send message back to client */
        	if (sendto(server_socket_fd, buffer, bytes_received, 0,(struct sockaddr *)&client_addr, client_addr_len)!=bytes_received) {
            		perror("sendto");
            		exit(EXIT_FAILURE);
        	}
	 	/*If bytes_received from client is zero :Client closed the connection */
                if (bytes_received == 0) {
                        printf("Client disconnected.\n");
                }

    }

    /* Close server socket */
    close(server_socket_fd);

    // Remove socket file
   // unlink(SOCKET_PATH);

    return 0;
}


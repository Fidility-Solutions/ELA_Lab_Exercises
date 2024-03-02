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

#define SOCKET_PATH "/tmp/datagram_socket"
#define BUF_SIZE 256

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
    	int client_socket_fd;

    	struct sockaddr_un server_addr, client_addr;
    	socklen_t server_addr_len = sizeof(struct sockaddr_un);
	/*buffer is used to take data from user and send it to server */
    	char buffer[BUF_SIZE];

	/* storage is used to capture the data from server */
    	char storage[BUF_SIZE];
    	ssize_t bytes_recived;

    	/* Create UNIX Domain Datagram client socket */
    	printf("Client socket created using socket () sys call ...\n");
    	client_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    	if (client_socket_fd == -1) {
        	perror("socket");
        	exit(EXIT_FAILURE);
    	}

    	/* Set up client address */
    	memset(&client_addr, 0, sizeof(struct sockaddr_un));
    	client_addr.sun_family = AF_UNIX;
   	snprintf(client_addr.sun_path, sizeof(client_addr.sun_path),"SOCKET_PATH.%ld", (long) getpid());

    	/* Bind the client socket with specified adress */
    	if (bind(client_socket_fd, (struct sockaddr *) &client_addr, sizeof(struct sockaddr_un)) == -1)
		perror("bind");

    	/* Construct address of server */
	memset(&server_addr, 0, sizeof(struct sockaddr_un));
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    	while (1) {
        	printf("Enter message to send (type 'exit' to quit): ");
        	fgets(buffer, sizeof(buffer), stdin);


        	/* Send message to server */
        	if (sendto(client_socket_fd, buffer, strlen(buffer), 0,(struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) != strlen(buffer)) {
           		perror("sendto");
            		exit(EXIT_FAILURE);
        	}

		/* Check if the user wants to exit */
                if (strncmp(buffer, "exit", 4) == 0) {
                        printf("Exiting...\n");
                        break;
                }
		 printf("Message sent successfully.\n");

		/* Receive messages from server */
		bytes_recived=recvfrom(client_socket_fd,storage,BUF_SIZE,0,NULL,NULL);
		if(bytes_recived==-1)
			perror("recvfrom");
		printf("Response from server  %.*s\n", (int) bytes_recived, storage);

    	}
    	remove(client_addr.sun_path); /* Remove client socket pathname */
	exit(EXIT_SUCCESS);

    	/* Close client socket */
	close(client_socket_fd);

    return 0;
}


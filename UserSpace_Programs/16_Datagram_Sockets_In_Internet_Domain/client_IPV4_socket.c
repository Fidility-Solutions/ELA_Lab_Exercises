/**************************************************************************
 * File:        client_IPV4_socket.c
 *
 * Description: This program demonstrates a simple Internet domain Datagram socket client
 *              using the socket API.It takes data from user to send to server from client,and 
 *              receives response from server and if user enter exit it will close the client socket.
 *
 * Usage:       ./client_IPV4_socket.c
 *
 * Author:      Fidility Solutions.
 *  
 * Date:        02/03/2024
 *
 * Reference:   The Linux Programming Interface book.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
/* Function: main
 *
 * Description: Entry point of the server program. Creates a Internet domain datagram socket server,
 *              takes input from user and send it to server, also receives response from server.
 *              and closes the client socket if user input is "exit".
 *
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */

int main() {
	printf("Welcome to the server-client application program using datagram sockets in the internet domain\n");
	/* Variable Declaration */
    	int client_fd;
    	struct sockaddr_in server_addr;
    	char message[MAX_BUFFER_SIZE];
    	char buffer[MAX_BUFFER_SIZE];

    	/* Create socket */
	printf("Client socket is created \n");
    	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	perror("Socket creation error");
        	exit(EXIT_FAILURE);
    	}
	/* clearing the structure before use */
    	memset(&server_addr, 0, sizeof(server_addr));

    	/* Set server address */
    	server_addr.sin_family = AF_INET;
    	server_addr.sin_port = htons(PORT);

    	/* Convert IPv4 address from text to binary form */
    	if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        	perror("inet_pton error");
        	exit(EXIT_FAILURE);
    	}

    	while (1) {
        	/* Take input from user */
        	printf("Enter message to send to server (or type 'exit' to quit): ");
        	if(fgets(message, MAX_BUFFER_SIZE, stdin)){

        		/* Send message to server */
       			if (sendto(client_fd, message, strlen(message), 0, (struct sockaddr *)&server_addr, sizeof(server_addr))==-1){
				perror("Sendto server fail \n");
				exit(EXIT_FAILURE);
			}

			/* Check if user wants to exit */
                	if (strncmp(message, "exit",4) == 0) {
                        	printf("User wants to exit\nExiting...\n");
                       	 	break;
                	}
		}

        	/* Receive response from server */
        	ssize_t recv_len = recvfrom(client_fd,buffer, MAX_BUFFER_SIZE, 0, NULL, NULL);
        	if (recv_len < 0) {
            		perror("Receive failed");
            		exit(EXIT_FAILURE);
        	}

        	/* Print response from server along with server's address */
        	buffer[recv_len] = '\0';
        	printf("Received from server at %s:%d: %s\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port), buffer);
    	}
	/*Close client */
    	close(client_fd);
    	return 0;
}



































































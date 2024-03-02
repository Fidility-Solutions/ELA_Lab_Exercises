/**************************************************************************
 * File:        server_IPV4_socket.c
 *
 * Description:	This program demonstrates a simple Internet domain Datagram socket server using the socket API. 
 * 		It creates a server socket, binds it to an Internet domain socket path, receives messages 
 * 		from clients, converts them to uppercase, sends them back to the client, and finally closes 
 * 		the server socket.
 *          
 * Usage:       ./server_IPV4_socket.c
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
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<ctype.h>
#define PORT 8080
#define MAX_BUFFER_SIZE 1024
/* Function: main
 *
 * Description:	Entry point of the server program. It creates an Internet domain socket server, 
 * 		receives client messages, sends responses to clients, and if the server needs 
 * 		any information from a client, it requests send to the client. Finally, it closes the server socket
 *             
 * Parameters:  None
 *
 * Return:      0 on successful execution, non-zero value on failure.
 */


int main() {
	printf("Welcome to the server-client application program using datagram sockets in the internet domain\n");
	/*Variable Declaration */
    	int server_fd;
    	struct sockaddr_in server_addr, client_addr;
    	socklen_t addr_len = sizeof(client_addr);
   	char client_buffer[MAX_BUFFER_SIZE];
	char server_buffer[MAX_BUFFER_SIZE];

    	/* Create socket */
	printf("The server socket is created \n");
    	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        	perror("Socket creation error");
        	exit(EXIT_FAILURE);
    	}
	/* clear the structure before proceed */
	printf("Clearing the structure before proceeding the communication...\n");
    	memset(&server_addr, 0, sizeof(server_addr));
    	memset(&client_addr, 0, sizeof(client_addr));

    	/* Load the server address */
	/* AF_INET is for IPV4 */
	printf("Loaded the sever family, address and port number to server socket \n");
    	server_addr.sin_family = AF_INET;
	/* Any type of IPV4 address */
    	server_addr.sin_addr.s_addr = INADDR_ANY;
	/* host to n/w byte order */
    	server_addr.sin_port = htons(PORT);

    	/* Bind socketwith specified address */
	printf("Binding the server socket with specified server address...\n");
    	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        	perror("Bind failed");
        	exit(EXIT_FAILURE);
    	}
	printf("Server Running...\n");
	printf("Server is ready to take data from clients\n");
    	while (1) {
        	/* Receive message from client */
        	ssize_t recv_len = recvfrom(server_fd, client_buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
        	if (recv_len < 0) {
            		perror("Receive failed");
            		exit(EXIT_FAILURE);
        	}

        	client_buffer[recv_len] = '\0';
        	/* Print client's message */
        	printf("Received from client at %s:%d: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_buffer);
		if(strncmp(client_buffer,"exit",4)==0)
			printf("Client %s exited\n",inet_ntoa(client_addr.sin_addr));
		else{
        		/* Change message to uppercase */
        		for (int i = 0;client_buffer[i] != '\0'; ++i) {
            			client_buffer[i] = toupper(client_buffer[i]);
        		}
        		/* Send uppercase message back to client */
        		if(sendto(server_fd,client_buffer, strlen(client_buffer), 0, (struct sockaddr *)&client_addr, addr_len)!=recv_len){
				perror("sendto:unable to send\n");
			}
		}
        	/* Check if server wants any information from client */
        	//printf("Enter message to send to client (or type 'exit' to quit): ");
        	if(fgets(server_buffer, MAX_BUFFER_SIZE, stdin)){
        	if (strncmp(server_buffer, "exit",4) == 0) {
            		printf("User Requested to exit\nExiting...\n");
            		break;
        	} 
		else {
            		if(sendto(server_fd, server_buffer, strlen(server_buffer), 0, (struct sockaddr *)&client_addr, addr_len)==-1){
				perror("Request fail to client\n");
        		}
    		}
		}
	}
	/* Close the server */
    	close(server_fd);

    	return 0;
}



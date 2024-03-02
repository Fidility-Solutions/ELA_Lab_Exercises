#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#define PORT 9640
#define BUF_SIZE 1024

int main() {
	printf("Welcome to Internet Domain Server-Client Datagram socket communcation \n");
	/* variable Declaration */
    	int sockfd;
    	struct sockaddr_in6 server_addr;
    	socklen_t addr_len;
    	char buffer[BUF_SIZE];
	char server_addr_str[INET6_ADDRSTRLEN]; // For presentation format
    
    	/* Create socket */
    	sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    	if (sockfd == -1) {
        	perror("Socket creation failed");
        	exit(EXIT_FAILURE);
    	}
	memset(&server_addr,0,sizeof(server_addr));
	/* Information for server */
    	server_addr.sin6_family = AF_INET6;
        server_addr.sin6_port = htons(PORT);

    	/* Initialize server address structure */
    	memset(&server_addr, 0, sizeof(server_addr));
    	if (inet_pton(AF_INET6, "::1", &server_addr.sin6_addr) <=0) {
		perror ("Invalid address/Address not supported\n");
		exit(EXIT_FAILURE);
    	}
       inet_ntop(AF_INET6, &server_addr.sin6_addr, server_addr_str, INET6_ADDRSTRLEN);
printf("Server address: %s\n", server_addr_str);       
    	while (1) {
        	/* Get user input */
        	printf("Enter message to send: ");
        	fgets(buffer, sizeof(buffer), stdin);
        
        	/* Send message to server */
        	addr_len = sizeof(server_addr);
        	if (sendto(sockfd, (char *)buffer, strlen(buffer), 0, (const struct sockaddr *)&server_addr, addr_len) == -1) {
            		perror("Send failed");
            		exit(EXIT_FAILURE);
        	}
		// Convert server address to presentation format
    		inet_ntop(AF_INET6, &server_addr.sin6_addr, server_addr_str, INET6_ADDRSTRLEN);
    		printf("Sending message to server %s:%d (%s)\n", server_addr_str, ntohs(server_addr.sin6_port), server_addr_str);
        
        	/* Receive response from server */
//        	ssize_t recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
        	socklen_t len=sizeof(server_addr);
		ssize_t recv_len;
		recv_len=recvfrom(sockfd,(char *)buffer,BUF_SIZE,MSG_WAITALL,(struct sockaddr *)&server_addr,&len);
		buffer[recv_len]='\0';
        	if (recv_len == -1) {
            		perror("Receive failed");
            		exit(EXIT_FAILURE);
        	}
		 // Convert server address to presentation format
    		inet_ntop(AF_INET6, &server_addr.sin6_addr, server_addr_str, INET6_ADDRSTRLEN);
    		printf("Received acknowledgment from server %s:%d (%s): %s\n", server_addr_str, ntohs(server_addr.sin6_port), server_addr_str, buffer);
        
        	/* Display response from server */
        	printf("Received response from server: %s\n", buffer);
    }
    
    /* Close the socket */
    close(sockfd);
    
    return 0;
}


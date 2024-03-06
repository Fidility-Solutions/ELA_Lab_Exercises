#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>
#define PORT 9640
#define BUF_SIZE 1024
#define MAX_ADDR_LEN INET6_ADDRSTRLEN
int main() {

    int sockfd;
    /* This structure allows us to transparently store either IPV4 or IPV6 socket address */
    struct sockaddr_in6 server_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUF_SIZE];
    char client_addr_str[MAX_ADDR_LEN];
    char server_addr_str[MAX_ADDR_LEN];
    
    /* Create socket */
    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    	/*clear the structure */
    	memset(&server_addr,0,sizeof(struct sockaddr_in6));
	memset(&client_addr,0,sizeof(struct sockaddr_in6));
    	/* information for server */
        server_addr.sin6_family = AF_INET6;
	/*accept packet from any IPV6 address */
	server_addr.sin6_addr=in6addr_any;
       	server_addr.sin6_port = htons(PORT);
    	/* Initialize server address structure */
    
    /* Bind socket to server ddress and port */
    addr_len = sizeof(server_addr);
    if (bind(sockfd, (struct sockaddr *)&server_addr, addr_len) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    if (inet_ntop(AF_INET6,&server_addr.sin6_addr,server_addr_str,INET6_ADDRSTRLEN) == NULL) {
            perror("Address conversion failed");
            exit(EXIT_FAILURE);
        }
	printf("the server adress %s\n",server_addr_str);
    
    printf("Server listening on port %d\n", PORT);
    /* Receive messages from client, convert them to uppercase , & return to client */
    while (1) {
        /* Receive message from client */
        addr_len = sizeof(client_addr);
        ssize_t recv_len = recvfrom(sockfd, buffer,BUF_SIZE,0, (struct sockaddr *)&client_addr, &addr_len);
	buffer[recv_len]='\0';
        if (recv_len == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }
	/* convert client address to string */
       if (inet_ntop(AF_INET6,&client_addr.sin6_addr,client_addr_str,INET6_ADDRSTRLEN) == NULL) {
            perror("Address conversion failed");
            exit(EXIT_FAILURE);
        }
        
        // Display received message and client address
	printf("server received %ld bytes from  %s:%u\n", (long)recv_len,client_addr_str,ntohs(client_addr.sin6_port));
        printf("Received message from client is %s:%s\n",client_addr_str,buffer);

	/* convert received string to uppercase */
	for (int j = 0; j < recv_len; j++)
		buffer[j] = toupper((unsigned char) buffer[j]);
        /* Echo back the message to the client */
        if (sendto(sockfd, buffer, recv_len,0, (struct sockaddr *)&client_addr, addr_len) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
    }
    
    // Close the socket
    close(sockfd);
    
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define FIFO_SERVER_TO_CLIENT "fifo_server_to_client" // FIFO for server to client
#define FIFO_CLIENT_TO_SERVER "fifo_client_to_server" // FIFO for client to server
#define BUF_SIZE 255

int main() {
    	int8_t fd_server_to_client, fd_client_to_server;
    	int8_t as8data[BUF_SIZE];
	int32_t as8ReadBuffer[BUF_SIZE];

    	/* Creating FIFOs if they don't exist */
    	mkfifo(FIFO_SERVER_TO_CLIENT, 0666);
    	mkfifo(FIFO_CLIENT_TO_SERVER, 0666);

    	/* Opening FIFOs for reading and writting */
    	fd_server_to_client = open(FIFO_SERVER_TO_CLIENT, O_RDWR); 
    	if (fd_server_to_client == -1) {
        	perror("open server to client");
        	exit(EXIT_FAILURE);
    	}
    	fd_client_to_server = open(FIFO_CLIENT_TO_SERVER, O_RDWR); 
    	if (fd_client_to_server == -1) {
        	perror("open client to server");
        	exit(EXIT_FAILURE);
    	}

    	printf("Server is running...\n");

    	/* Reading and writing data between client and server */
    	while(1){
	    	while (fgets((char *)as8data, BUF_SIZE, stdin) != NULL){
        		/* Write data to server */
        		if(write(fd_server_to_client, (char *)as8data, strlen(as8data)) == -1){
				perror("write to client error");
				break;
	    		}
			if(strncmp(as8data,"exit\n",4) == 0){
				printf("User Entered Exit, Exiting...\n");
				break;
			}
			break;
		}

        	ssize_t bytes_read;
        	/* Read from client to server */
        	bytes_read = read(fd_client_to_server, as8ReadBuffer, BUF_SIZE);
        	if(bytes_read == -1){
            		perror("read client to server error");
            		exit(EXIT_FAILURE);
        	} 
        	printf("Received from client (to server): %s", (char *)as8ReadBuffer);
		as8ReadBuffer[bytes_read]='\0';

        	/* Modify data (e.g., to uppercase) */
        	for (int i = 0; i < bytes_read; i++)
            		as8ReadBuffer[i] = toupper(as8ReadBuffer[i]);

        	/* Write modified data to client */
       		if(write(fd_server_to_client,as8ReadBuffer, BUF_SIZE)== -1){
       			perror("write to client fail ");
			exit(EXIT_FAILURE);
		}
    	}

    /* Close FIFOs */
    close(fd_server_to_client);
    close(fd_client_to_server);
    unlink(FIFO_SERVER_TO_CLIENT);
    unlink(FIFO_CLIENT_TO_SERVER);
    return 0;
}


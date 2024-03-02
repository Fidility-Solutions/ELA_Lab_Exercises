#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#define FIFO_SERVER_TO_CLIENT "fifo_server_to_client" // FIFO for server to client
#define FIFO_CLIENT_TO_SERVER "fifo_client_to_server" // FIFO for client to server

int main() {
    int fd_server_to_client, fd_client_to_server;
    char data[BUFSIZ];

    // Creating FIFOs if they don't exist
    mkfifo(FIFO_SERVER_TO_CLIENT, 0666);
    mkfifo(FIFO_CLIENT_TO_SERVER, 0666);

    // Opening FIFOs
    fd_server_to_client = open(FIFO_SERVER_TO_CLIENT, O_RDWR); // Open for reading and writing
    if (fd_server_to_client == -1) {
        perror("open server to client");
        exit(EXIT_FAILURE);
    }
    fd_client_to_server = open(FIFO_CLIENT_TO_SERVER, O_RDWR); // Open for reading and writing
    if (fd_client_to_server == -1) {
        perror("open client to server");
        exit(EXIT_FAILURE);
    }

    printf("Server is running...\n");

    // Reading and writing data between client and server
    while (1) {
	    while (fgets(data, BUFSIZ, stdin) != NULL) {
        // Write data to server
        write(fd_server_to_client, data, strlen(data));
	break;
	    }
        ssize_t bytes_read;

        // Read from client to server
        bytes_read = read(fd_client_to_server, data, BUFSIZ);
        if (bytes_read == -1) {
            perror("read client to server");
            exit(EXIT_FAILURE);
        } else if (bytes_read == 0) {
            printf("Client closed the connection.\n");
            break; // Exit the loop if client closes the connection
        }
        printf("Received from client (to server): %s", data);

        // Modify data (e.g., to uppercase)
        for (int i = 0; i < bytes_read; i++) {
            data[i] = toupper(data[i]);
        }

        // Write modified data to client
        write(fd_server_to_client, data, BUFSIZ);

        // Read from server to client

    }

    // Close FIFOs
    close(fd_server_to_client);
    close(fd_client_to_server);
    unlink(FIFO_SERVER_TO_CLIENT);
    unlink(FIFO_CLIENT_TO_SERVER);
    return 0;
}


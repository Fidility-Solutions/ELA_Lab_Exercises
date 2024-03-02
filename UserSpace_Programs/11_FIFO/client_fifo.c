#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define FIFO_SERVER_TO_CLIENT "fifo_server_to_client" // FIFO for server to client
#define FIFO_CLIENT_TO_SERVER "fifo_client_to_server" // FIFO for client to server

int main() {
    int fd_server_to_client, fd_client_to_server;
    char data[BUFSIZ];

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

    printf("Enter message to send (type 'exit' to quit):\n");

    // Reading input from user and sending to server
    while (fgets(data, BUFSIZ, stdin) != NULL) {
        // Write data to server
        write(fd_client_to_server, data, strlen(data));

        // Read from server
        ssize_t bytes_read = read(fd_server_to_client, data, BUFSIZ);
        if (bytes_read == -1) {
            perror("read server to client");
            exit(EXIT_FAILURE);
        } else if (bytes_read == 0) {
            printf("Server closed the connection.\n");
            break; // Exit the loop if server closes the connection
        }
        printf("Received from server: %s", data);

        printf("Enter message to send (type 'exit' to quit):\n");
    }

    // Close FIFOs
    close(fd_server_to_client);
    close(fd_client_to_server);
    return 0;
}


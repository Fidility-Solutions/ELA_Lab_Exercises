#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<errno.h>
#include <sys/stat.h>


#define SERVER_FIFO "/tmp/server_fifo_file"
#define CLIENT_FIFO_TEMPLATE "/tmp/client_fifo_file.%d"
#define MAX_BUFFER_SIZE 256
#define CLIENT_FIFO_NAME_LEN 256

/* creating structure of request message and response message to exchange data between client & server */
/* Request (client --> server) */
struct Request{
	/* pid of client */
        pid_t client_pid;
	/* Length of desired sequence */
        int num_request;
};

/* structure for response time */
/* Response (server --> client) */
struct Response{
	/* Start of sequence */
        int start_seq_num;
};


#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#define SERVER_FIFO "/tmp/server_fifo"

/* Well-known name for server's FIFO */
#define CLIENT_FIFO_TEMPLATE "/tmp/client_fifo.%ld"

/* Template for building client FIFO name */
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

/* Space required for client FIFO pathname(+20 as a generous allowance for the PID) */
struct request{                 /* Request (client --> server) */
        pid_t pid;              /* PID of client */
        int ReqNum;             /* Requst number to server */
};
struct response {               /* Response (server --> client) */
        int RespNum;            /* Start of sequence */
};

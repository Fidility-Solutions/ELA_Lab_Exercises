#include "fifo.h" 
// Define GN_GT_0 if not already defined
#ifndef GN_GT_0
#define GN_GT_0 1
#endif
static char clientFifo[CLIENT_FIFO_NAME_LEN];
/* Invoked on exit to delete client FIFO */
static void removeFifo(void)
{
	unlink(clientFifo);
}
int main(int argc, char *argv[])
{
	int serverFd, clientFd;
	struct request req;
	struct response resp;
	if (argc > 1 && strcmp(argv[1], "--help") == 0)
		fprintf(stderr,"%s [seq-len...]\n", argv[0]);

	/* Create our FIFO (before sending request, to avoid a race) */

	umask(0); /* So we get the permissions we want */
	snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE,(long) getpid());
	if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
		fprintf(stderr,"mkfifo %s", clientFifo);
	if (atexit(removeFifo) != 0)
		perror("atexit");

	/* Construct request message, open server FIFO, and send request */
	req.pid = getpid();
	if(argc > 1) {
        char *endptr;
        long int value = strtol(argv[1], &endptr, 10);
        if(*endptr == '\0' && value > 0) {
		req.seqLen = (int)value;
        } 
	else{
            fprintf(stderr, "Invalid sequence length. Using default value.\n");
        }
    }
	serverFd = open(SERVER_FIFO, O_WRONLY);
	if (serverFd == -1)
		fprintf(stderr,"open %s", SERVER_FIFO);
	if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
		perror("Can't write to server");
	
	/* Open our FIFO, read and display response */
	clientFd = open(clientFifo, O_RDONLY);
	if(clientFd == -1)
		fprintf(stderr,"open %s", clientFifo);
	if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
		perror("Can't read response from server");
	printf("%d\n", resp.seqNum);
	exit(EXIT_SUCCESS);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include<string.h>
#define MAX_MSG_SIZE 256
#define QUEUE_NAME "/my_queue"
#define PERMISSIONS 0660

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buf[MAX_MSG_SIZE];
    pid_t pid;

    // Set up message queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10; // Maximum number of messages in queue
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Create or open the message queue
    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // Forking the process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        while (1) {
		/* Check if the received message is "exit" */
            if (mq_receive(mq, buf, MAX_MSG_SIZE, NULL) == -1) {
                perror("mq_receive");
                exit(EXIT_FAILURE);
            }
	    if (strncmp(buf, "exit", 4) == 0) {
            printf("Received 'exit', exiting...\n");
	    break;
        }
            printf("Child received: %s\n", buf);
        }
    } else { // Parent process
        while (1) {
            printf("Enter a message: ");
            fgets(buf, MAX_MSG_SIZE, stdin);

            // Remove trailing newline character
            size_t len = strlen(buf);
            if (len > 0 && buf[len - 1] == '\n')
                buf[len - 1] = '\0';

            // Send message to child
            if (mq_send(mq, buf, strlen(buf) + 1, 0) == -1) {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }
	    if (strncmp(buf, "exit", 4) == 0) {
            printf("Received 'exit', exiting...\n");
	    break;
        }

        }
    }

    // Close and unlink the message queue
    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    return 0;
}


/******************************************************************************
 * File:        message_queue.c
 *
 * Description: This program demonstrates inter-process communication using
 *              POSIX message queues. It creates a message queue, sets up
 *              a notification mechanism for message arrival, forks a child
 *              process, sends a message from the parent process to the
 *              message queue, receives the message in the child process,
 *              and finally closes and unlinks the message queue.
 *
 * Usage:       ./message_queue
 * 
 * Author:      Fidility Solutions.
 *  
 * Date:        29/02/2024.
 *
 * Reference    The Linux Programming Interface book
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>
#include <errno.h>

#define QUEUE_NAME "/my_queue"
#define MAX_MSG_SIZE 256
#define MAX_MSG_COUNT 10
/*
 * Function: 	signal_handler
 * 
 * Description:	Signal handler function to handle notification signal.
 *
 * Argument:	signum:The signal number.
 *
 */
void signal_handler(int signum) {
    printf("Notification received!\n");
}
/* 
 * Function: 	main
 *
 *Description:	Entry point of the program. It registers a signal handler, forks a child process, 
 		sends a message from the parent process to the message queue, receives the message 
		in the child process, closes the message queue, and unlinks it.
 *
 *Parameters: 	NONE 

 * Returns:     0 upon successful execution of the program.
 */
int main() {
    mqd_t mqd;
    /* Message queue attributes */
    struct mq_attr attr;
    /* No special flags */
    attr.mq_flags = 0;    
    /* Max number of messages in the queue */
    attr.mq_maxmsg = MAX_MSG_COUNT; 
    /* Max message size */
    attr.mq_msgsize = MAX_MSG_SIZE;

    /* Create a message queue with specified attributes */
    mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mqd == (mqd_t)-1) {
        perror("mq_open");
        exit(EXIT_FAILURE);
    }
    printf("Message queue created.\n");

    /* Set up notification: allow a process to be notified when a message is available in a queue */
    struct sigevent sev;
    /* Type of notification by sending a signal */
    sev.sigev_notify = SIGEV_SIGNAL;
    /* Signal value to be sent if notification type is SIGEV_SIGNAL */
    sev.sigev_signo = SIGUSR1;
    /* Using 'mq_notify' to send a notification signal (SIGUSR1) when a message is available in the message queue */
    if (mq_notify(mqd, &sev) == -1) {
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }
    printf("Notification mechanism set up.\n");

    /* Register signal handler for notification signal */
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sa.sa_flags = 0;
    /* Clear signal mask during signal handler execution */
    sigemptyset(&sa.sa_mask); 
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    printf("Signal handler registered.\n");

    /* Fork a child process */
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    /* Child process */
    if (pid == 0) { 
        /* Receive messages from queue */
        printf("Child process waiting for messages...\n");
        char received_message[MAX_MSG_SIZE];
        unsigned int priority;
        ssize_t received_bytes = mq_receive(mqd, received_message, MAX_MSG_SIZE, &priority);
        if (received_bytes == -1) {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
        /* Null terminate the received message */
        received_message[received_bytes] = '\0'; 
        printf("Received message in child process: %s\n", received_message);
    } else { /* Parent process */
        /* Send a message to queue */
        char message[] = "Hello, Message Queue from Parent!";
        if (mq_send(mqd, message, strlen(message) + 1, 0) == -1) {
            perror("mq_send");
            exit(EXIT_FAILURE);
        }
        printf("Message sent by parent process.\n");
    }

    /* Close the message queue */
    if (mq_close(mqd) == -1) {
        perror("mq_close");
        exit(EXIT_FAILURE);
    }
    printf("Message queue closed.\n");
/* Introduce a short delay to ensure proper cleanup */
	sleep(3); 
    /* Unlink the message queue */
    if (mq_unlink(QUEUE_NAME) == -1) {
        perror("mq_unlink");
        exit(EXIT_FAILURE);
    }
    printf("Message queue unlinked.\n");

    return 0;
}


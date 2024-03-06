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
#include <stdint.h>

#define QUEUE_NAME "/my_queue"
#define MAX_MSG_SIZE 256
#define MAX_MSG_COUNT 10
/*
 * Function: 	signal_handler
 * 
 * Description:	Signal handler function to handle notification signal.
 *
 * Argument:	s8SigNum:The signal number.
 *
 */
void signal_handler(int8_t s8SigNum) {
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

void MsgQHandler(){
	mqd_t MsgQDescriptor;

        /* Message queue attributes */
        struct mq_attr StrAttr;

        printf("Setting Attributes to Structure attr with SIZE and Maximum Message Count\n");
        /* No special flags */
        StrAttr.mq_flags = 0;

        /* Max number of messages in the queue */
        StrAttr.mq_maxmsg = MAX_MSG_COUNT;

        /* Max message size */
        StrAttr.mq_msgsize = MAX_MSG_SIZE;

        /* Create a message queue with specified attributes */
        printf("Creating Message Queue with specified attributes... \n");
        MsgQDescriptor = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &StrAttr);
        if (MsgQDescriptor == (mqd_t)-1) {
                perror("mq_open error");
                exit(EXIT_FAILURE);
        }



}

int main(void) {
	printf("Welcome to POSIX Message Queue Program\n");
    	mqd_t MsgQDescriptor;
    
	/* Message queue attributes */
    	struct mq_attr StrAttr;
    	
	printf("Setting Attributes to Structure attr with SIZE and Maximum Message Count\n");
	/* No special flags */
    	StrAttr.mq_flags = 0;    
    
	/* Max number of messages in the queue */
    	StrAttr.mq_maxmsg = MAX_MSG_COUNT; 
    
	/* Max message size */
    	StrAttr.mq_msgsize = MAX_MSG_SIZE;

    	/* Create a message queue with specified attributes */
	printf("Creating Message Queue with specified attributes... \n");
    	MsgQDescriptor = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &StrAttr);
    	if (MsgQDescriptor == (mqd_t)-1) {
        	perror("mq_open error");
        	exit(EXIT_FAILURE);
    	}
    	printf("Message queue is created.\n");

    	/* Set up notification: allow a process to be notified when a message is available in a queue */
    	printf("Setting Notification Mechanism to be notified when a message is available in a queue...\n");

    	struct sigevent sev;
    
	/* Type of notification by sending a signal */
    	sev.sigev_notify = SIGEV_SIGNAL;
    
	/* Signal value to be sent if notification type is SIGEV_SIGNAL */
    	sev.sigev_signo = SIGUSR1;
    
	/* Using 'mq_notify' to send a notification signal (SIGUSR1) when a message is available in the message queue */
    	if (mq_notify(MsgQDescriptor, &sev) == -1) {
        	perror("mq_notify error");
        	exit(EXIT_FAILURE);
    	}
    	printf("Notification mechanism set up completed.\n");

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
        	printf("This is Child Process and child process waiting for messages...\n");
        	int8_t as8RecvMsgBuf[MAX_MSG_SIZE];
        	uint32_t u8Priority;
        	ssize_t RecvBytes = mq_receive(MsgQDescriptor, as8RecvMsgBuf, MAX_MSG_SIZE, &u8Priority);
        	if (RecvBytes == -1) {
            		perror("mq_receive error");
            		exit(EXIT_FAILURE);
		}

        	/* Null terminate the received message */
        	as8RecvMsgBuf[RecvBytes] = '\0'; 
        	printf("Received message in child process: %s\n", as8RecvMsgBuf);
    	}
       /* Parent Process */	
	else{
  
		/* Send a message to queue */
        	int8_t as8MsgToSend[] = "Hello, Message Queue from Parent!";
        	if (mq_send(MsgQDescriptor, as8MsgToSend, strlen(as8MsgToSend) + 1, 0) == -1) {
            		perror("mq_send error");
            		exit(EXIT_FAILURE);
        	}
        	printf("Message sent by parent process.\n");
    	}

    	/* Close the message queue */
    	if(mq_close(MsgQDescriptor) == -1) {
        	perror("mq_close error");
        	exit(EXIT_FAILURE);
    	}
    	printf("Message queue closed.\n");

	/* Introduce a short delay to ensure proper cleanup */
	sleep(3); 
    
	/* Unlink the message queue */
    	if(mq_unlink("/my_queue") == -1) {
        	perror("mq_unlink error");
        	exit(EXIT_FAILURE);
    	}
    	printf("Message queue unlinked.\n");

    	return 0;
}


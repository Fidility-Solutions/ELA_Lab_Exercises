/******************************************************************************
 * File		: message_queue.c
 *
 * Description	: This program demonstrates inter-process communication using POSIX message queues. 
 * 		 It creates a message queue, sets up a notification mechanism for message arrival, 
 * 		 forks a child process, sends a message from the parent process to the message queue, 
 * 		 receives the message in the child process and finally closes and unlinks the message queue.
 *
 * Usage	: ./message_queue
 * 
 * Author	: Fidility Solutions.
 *  
 * Date 	: 29/02/2024.
 *
 * Reference    : The "Linux Programming Interface" book.
 *
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <signal.h>

#define QUEUE_NAME "/my_queue"
#define MAX_MSG_SIZE 1024
#define MSG_PRIORITY 1

mqd_t mq;
char write_buffer[MAX_MSG_SIZE];
char read_buffer[MAX_MSG_SIZE];

/* Function to re-register for message queue notifications */
void register_notification(void) {
	/* Set up notification: allow a process to be notified when a message is available in a queue */
   	 struct sigevent sev;
	/* Type of notification by sending a signal */
   	 sev.sigev_notify = SIGEV_SIGNAL;
	 /* Signal value to be sent if notification type is SIGEV_SIGNAL */
   	 sev.sigev_signo = SIGUSR1;
   	 sev.sigev_value.sival_ptr = &mq;
	/* Using 'mq_notify' to send a notification signal (SIGUSR1) when a message is available in the message queue */
   	 if (mq_notify(mq, &sev) == -1) {
   	     	perror("mq_notify fail");
   	     	exit(EXIT_FAILURE);
   	 }
}



/* Signal handler function to be called when a message arrives */
void handle_signal(int sig) {
    	ssize_t bytes_read;

    	/* Receive the message from the queue */
    	bytes_read = mq_receive(mq, read_buffer, MAX_MSG_SIZE, NULL);
    	if (bytes_read >= 0) {
    	    read_buffer[bytes_read] = '\0';
    	    printf("\nChild process received message: %s\n", read_buffer);
    	} else {
    	    perror("mq_receive fail");
    	    exit(EXIT_FAILURE);
    	}

    /* Re-register notification */
    register_notification();
}


/* 
 * Function:    main
 *
 * Description: Entry point of the program. It registers a signal handler, forks a child process, 
                sends a message from the parent process to the message queue, receives the message 
                in the child process, closes the message queue and unlinks it.
 *
 * Parameters:  NONE 

 * Returns:     0 upon successful execution of the program.
 */


int main(void){
	printf("Welcome to POSIX Message Queue Program\n");
	/* Message queue attributes */
   	 struct mq_attr attr;
   	 pid_t pid;

   	 /* Set up attributes of the message queue */
	/* No special flags */
   	 attr.mq_flags = 0;
	 /* Max number of messages in the queue */
   	 attr.mq_maxmsg = 10;
	/* Max message size */
   	 attr.mq_msgsize = MAX_MSG_SIZE;
   	 attr.mq_curmsgs = 0;

   	 // Create a message queue
   	 mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
   	 if(mq == (mqd_t)-1){
   	     	perror("mq_open fail");
   	     	exit(EXIT_FAILURE);
   	 }
	/* Fork a child process */
   	 pid = fork();

   	 if(pid == -1){
   	     	perror("fork fail");
   	     	exit(EXIT_FAILURE);
   	 } 
	 else if (pid == 0){
   	     	/* Child process (Receiver) */
		 printf("\nThe Child Process is created\n");

   	     	/* Register signal handler for notification using sigaction */
   	     	struct sigaction sa;
   	     	sa.sa_handler = handle_signal;
   	     	sigemptyset(&sa.sa_mask);
   	     	sa.sa_flags = 0;
   	     	if(sigaction(SIGUSR1, &sa, NULL) == -1) {
   	         	perror("sigaction fail");
   	         	exit(EXIT_FAILURE);
   	     }

   	     /* Request notification for SIGUSR1 signal */
   	     register_notification();
	     printf("Signal handler registered for notification.\n");

   	     printf("Child process waiting for message...\n");
   	     while (1){
		     /* Wait for signal */
   	         	pause(); 
   	     }
   	 } 
	 else {
   	     /* Parent process (Sender) */
		 sleep(3);
   	     printf("\nParent process sending message...\n");

   	     /* Prepare message */
   	     strcpy(write_buffer, "Hello, child process!");
   	     if (mq_send(mq, write_buffer, strlen(write_buffer) + 1, MSG_PRIORITY) == -1) {
   	         perror("mq_send fail");
   	         exit(EXIT_FAILURE);
   	     }

   	     printf("Parent process sent message.\n");
   	 }

   	 /* Close and unlink the message queue */
   	 if(pid != 0){  /* Ensure the parent process cleans up the message queue */
   	     	mq_close(mq);
   	     	mq_unlink(QUEUE_NAME);
   	 }

   	 return 0;
}


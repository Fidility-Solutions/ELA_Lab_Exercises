/******************************************************************************
 * File:        User_MsgQueue.c
 *
 * Description: This program demonstrates inter-process communication using POSIX message queues. 
 * 		It creates a message queue, forks a child process, sends a message from the parent 
 * 		process to the message queue, receives the message in the child process,
 *              and finally closes and unlinks the message queue when the user enter 'exit'.
 *
 * Usage:       ./User_MsgQueue.c
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <stdint.h>
#include <sys/wait.h>

#define MAX_MSG_SIZE 256
#define QUEUE_NAME "/USER_POSIX_MQ"
#define PERMISSIONS 0660
/*
 * Function:	errExit
 *
 * Description:	This function will notify type of error which is  occured in the progarm and exit from program
 *
 * Parameters:	const char *message
 *
 * Return:	None
 *
 * */
void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}


/* 
 * Function:    main
 *
 *Description:  Entry point of the program. It forks a child process, takes data from user and
                sends it from the parent process to the message queue, the child process recieves the message 
                from message queue when it is available, finally when user enter 'exit' that closes the message queue 
		and unlinks it.
 *
 *Parameters:   NONE 

 * Returns:     0 upon successful execution of the program.
 */

int main(void){
	printf("Welcome to POSIX Message Queue\n");
	/* variable declaration */
    	mqd_t MsgQDescriptor;
    	struct mq_attr StrAttr;
    	int8_t as8RecvBuffer[MAX_MSG_SIZE];
	int8_t as8SendBuffer[MAX_MSG_SIZE];
    	pid_t pid;

    	/* Set up message queue attributes */
    	StrAttr.mq_flags = 0;
	/* Maximum number of messages in queue & maxim message size*/
    	StrAttr.mq_maxmsg = 10;
    	StrAttr.mq_msgsize = MAX_MSG_SIZE;
    	StrAttr.mq_curmsgs = 0;

    	/* Create or open the message queue */
	printf("The message Queue is created\n");
    	MsgQDescriptor = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &StrAttr);
    	if(MsgQDescriptor == (mqd_t)-1)
		errExit("message queue open error");

    	/* Forking the process */
    	pid = fork();
    	if(pid == -1)
		errExit("fork error");

	/*child process */
    	if(pid == 0){
		printf("\nChild Process created\n");
        	for(;;){
            		if(mq_receive(MsgQDescriptor, as8RecvBuffer, MAX_MSG_SIZE, NULL) == -1)
                		errExit("mq_receive error");

			/* Check if the received message is "exit" */
	    		if(strncmp(as8RecvBuffer, "exit", 4) == 0){
            			printf("Received 'exit' from parent, exiting...\n");
	    			break;
        		}
            		printf("\nChild  process received: %s\n", as8RecvBuffer);	
    		}
		exit(EXIT_SUCCESS);
	}	
	/* parent process */
	else{
		sleep(1);
	       printf("\nThis is Parent process which sends data to message queue \n");	
//	       printf("\nEnter a message(exit to 'quit'):\n");
        	for(;;){
			sleep(1);
			printf("\nEnter a message(exit to 'quit'):");

			/*Taking Input from User */
            		fgets(as8SendBuffer, MAX_MSG_SIZE, stdin);

            		/* Remove trailing newline character */
            		size_t LenOfMsg = strlen(as8SendBuffer);
            		if(LenOfMsg > 0 && as8SendBuffer[LenOfMsg - 1] == '\n')
                		as8SendBuffer[LenOfMsg - 1] = '\0';

            		/* Send message to child */
            		if(mq_send(MsgQDescriptor, as8SendBuffer, strlen(as8SendBuffer) + 1, 0) == -1)
                		errExit("mq_send error");

	    		if(strncmp(as8SendBuffer, "exit", 4) == 0){
	    			break;
        		}
        	}
		/* wait for child to terminate */
		wait(NULL);
		printf("Exiting from parent process...\n");
    	}

    	/* Close and unlink the message queue */
    	mq_close(MsgQDescriptor);
    	mq_unlink(QUEUE_NAME);

    	return 0;
}


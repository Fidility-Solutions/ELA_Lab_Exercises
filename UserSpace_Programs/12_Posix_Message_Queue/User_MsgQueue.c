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
#define QUEUE_NAME "/my_queue"
#define PERMISSIONS 0660


/* 
 * Function:    main
 *
 *Description:  Entry point of the program. It forks a child process, takes data from user and
                sends it from the parent process to the message queue, the child process recieves the message 
                from message queue when it available, finally when user enter 'exit' that closes the message queue, 
		and unlinks it.
 *
 *Parameters:   NONE 

 * Returns:     0 upon successful execution of the program.
 */

int main(){
	printf("Welcome POSIX Message Queue\n");
	/* variable declaration */
    	mqd_t mqd;
    	struct mq_attr attr;
    	int8_t as8RecvBuffer[MAX_MSG_SIZE];
	int8_t as8SendBuffer[MAX_MSG_SIZE];
    	pid_t pid;

    	/* Set up message queue attributes */
    	attr.mq_flags = 0;
	/* Maximum number of messages in queue & maxim message size*/
    	attr.mq_maxmsg = 10;
    	attr.mq_msgsize = MAX_MSG_SIZE;
    	attr.mq_curmsgs = 0;

    	/* Create or open the message queue */
	printf("The message Queue is created\n");
    	mqd = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, PERMISSIONS, &attr);
    	if(mqd == (mqd_t)-1){
        	perror("mq_open error");
        	exit(EXIT_FAILURE);
    	}

    	/* Forking the process */
    	pid = fork();
    	if(pid == -1){
        	perror("fork");
        	exit(EXIT_FAILURE);
    	}
	
	/*child process */
    	if(pid == 0){
		printf("\nchild Process created\n");
        	while(1){
            		if(mq_receive(mqd, as8RecvBuffer, MAX_MSG_SIZE, NULL) == -1){
                		perror("mq_receive");
                		exit(EXIT_FAILURE);
            		}
			/* Check if the received message is "exit" */
	    		if(strncmp(as8RecvBuffer, "exit", 4) == 0){
            			printf("Received 'exit' from parent, exiting...\n");
	    			break;
        		}
            		printf("Child received: %s\n", as8RecvBuffer);	
    		}
		exit(EXIT_SUCCESS);
	}	
	/* parent process */
	else{
	       printf("\nThis is Parent process it will send data to message queue \n");	
	       printf("\nEnter a message:\n");
        	while(1){
            		fgets(as8SendBuffer, MAX_MSG_SIZE, stdin);

            		/* Remove trailing newline character */
            		size_t LenOfMsg = strlen(as8SendBuffer);
            		if(LenOfMsg > 0 && as8SendBuffer[LenOfMsg - 1] == '\n')
                		as8SendBuffer[LenOfMsg - 1] = '\0';

            		/* Send message to child */
            		if(mq_send(mqd, as8SendBuffer, strlen(as8SendBuffer) + 1, 0) == -1){
                		perror("mq_send error");
                		exit(EXIT_FAILURE);
            		}
	    		if(strncmp(as8SendBuffer, "exit", 4) == 0){
            			printf("Received 'exit', exiting...\n");
	    			break;
        		}
        	}
		/* wait for child to terminate */
		wait(NULL);
    	}

    	/* Close and unlink the message queue */
    	mq_close(mqd);
    	mq_unlink(QUEUE_NAME);

    	return 0;
}


/******************************************************************************
 * File		: Thread_IDs.c
 *
 * Description	: This file contains a C program demonstrating the creation of multiple thread  and printing 
 * 		  thread IDs with respective Therads. 
 *
 * Author	: Fidility Solutions.
 *
 * Date		: 26/02/2024.
 *
 * Reference   	: "The Linux Programming Interface" book.

* *******************************************************************************/
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include<sys/syscall.h>

/*
 * Function:    threadfunction()
 *
 * Description: This function represents the code executed by each thread. It receives
 *              a thread ID and a message as arguments. Based on the message received,
 *              it performs different operations.
 *
 * Parameters: 	arg: A pointer to void representing the arguments passed to the thread function.
 *           	The first sizeof(int) bytes contain the thread ID, and the remaining bytes
 *           	contain the message.
 * 
 * Returns:     NULL: Since the function is of type void *, it returns NULL to indicate successful completion 
 *            of the thread function.
 *
 */
void *threadfunction(void *arg) {

	/* Struct to hold thread arguments */
	struct ThreadArgs {
    		int thread_number;
    		const char *message;
	};
 	/* Get the thread ID (TID) of the current thread. */
        pid_t thread_id = syscall(SYS_gettid);

    	/* Extract arguments passed to the thread */
    	struct ThreadArgs *thread_args = (struct ThreadArgs *)arg;
	printf("\n");
    	/* Printing thread id and the message based on the thread number */
    	if (thread_args->thread_number == 0) {
        	printf("Thread_%d is running with Thread ID:%d and the argument passed to the thread_1 is:\"%s\" \n",thread_args->thread_number,thread_id,thread_args->message);
		sleep(5);
    	} 
	else if (thread_args->thread_number == 1) {
        	printf("Thread_%d is running with Thread ID:%d and the argument passed to the thread_2 is:\"%s\" \n",thread_args->thread_number,thread_id,thread_args->message);
		sleep(5);
  
    	}
	else if(thread_args->thread_number==2){
		printf("Thread_%d is running with Thread ID:%d and the argument passed to the thread_3 is:\"%s\" \n",thread_args->thread_number,thread_id,thread_args->message); 
		sleep(5);
	}

    	else {
       		printf("Thread ID:%d says Unknown message: %s\n",thread_id, thread_args->message);
		sleep(5);
    }
    /* Return a value indicating successful execution */
    return NULL;
}
/*
 * Function:    main()
 *
  * Description:This function represents the entry point of the program. It creates
 *              multiple threads, each with its own unique arguments, and waits for
 *              all threads to finish execution.
 *
 * Parameters:   None
 *  
 * Returns:     0: Indicates successful execution of the program.
 *      	1: Indicates an error occurred during thread creation or joining.
 *
 */


int main(void) {
    	printf("Demonistation of thread IDs\n");
	pid_t Process_id=getpid();
	printf("This is main Thread with PID:%d\n",Process_id);

	/* Array to hold thread IDs */
    	pthread_t threads[3]; 

	/* Struct to hold thread arguments */
	struct thread_args_struct {
        	int thread_number;
        	char *message;
    	} thread_args[3] = {
        	{0, "Hello"},
        	{1, "Welcome"},
        	{2, "Hi"}
    		}; 

    	/* Creating multiple threads */
    	for (int i = 0; i < 3; i++) {
        	if (pthread_create(&threads[i], NULL, threadfunction, (void *)&thread_args[i]) != 0) {
            	fprintf(stderr, "Error creating thread %d.\n", i);
            	return 1;
        	}
    	}

    	/* Wait for all threads to finish */
    	for (int i = 0; i < 3; i++) {
		int threadend;
				 if ((threadend = pthread_join(threads[i], NULL)) == 0)
				{
				printf("Thread_%d is exited\n",i);
				sleep(3);
				}
		else {
            		fprintf(stderr, "Error joining thread_%d.\n", i + 1);
            		return 1;
        	}
    	}
    	printf("Exiting Main thread.\n");
    	return 0;
}


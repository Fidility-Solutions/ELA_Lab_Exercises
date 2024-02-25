/*******************************************************************************
 * File:        Thread_Termination.c
 *
 * Description: This file contains a C program demonstrating the creation and termination of thread
 *              using the pthreads library. 
 *
 * Author:      Fidility Solutions.
 *
 * Date:        23/02/2024.
 *
 * Reference    The Linux Programming Interface book

* *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include<sys/syscall.h>

/*
 * Function:    threadfunction()
 *
 * Description: This function serves as the entry point for a new thread. It prints various details 
 *              about the thread IDs.
 *
 * Arguments:   None.
 * 
 * Returns:     NULL: Since the function is of type void *, it returns NULL to indicate successful completion 
 *              of the thread function.
 *
 */

void *threadfunction(void *arg) 
{
	printf("New Thread function started...\n New Thread ID(TID):%ld\n",syscall(SYS_gettid));
	/* Simulating some work done by the thread */
	sleep(3); 
	printf("New Thread function exiting...\n");
	/* Terminating the thread */
	pthread_exit(NULL);
}
/*
 * Function:    main()
 *
 * Description: This is the entry point of the program. It demonstrates the creation and termination
 *              of a thread using the pthreads library. This function shows creates
 *              a new thread, waits for the new thread to finish its execution, and then exits.
 *
 * Arguments:   None
 *
 * Returns:     0 on successful execution, 1 on failure.
 *
 */
int main() 
{
	printf("Entered into main program\n");

	/* Variables Declaration */
    	pthread_t thread;
    	int thread_create_status;

    	/* Create a thread */
    	thread_create_status = pthread_create(&thread, NULL, threadfunction, NULL);

	/* Check if thread creation was successful */
    	if (thread_create_status != 0) {
        	fprintf(stderr, "Error creating thread.\n");
        	exit(EXIT_FAILURE);
    	}
	 /* Main thread continues execution while the other thread runs */
	printf("This is Main Thread with ID(TID):%ld\n",syscall(SYS_gettid));

    	/* Wait for the created thread to finish */
	/* pthread_join blocks the main thread until the specified thread terminates */
    	if (pthread_join(thread, NULL) != 0) {
        	fprintf(stderr, "Error joining thread.\n");
        	exit(EXIT_FAILURE);
    	}
	/* Main thread exiting */
    	printf("Main Thread exiting...\n");
    	return 0;
}

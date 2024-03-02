/*******************************************************************************
 * File:        Thread_Termination.c
 *
 * Description: This file contains a C program demonstrating the creation and termination of thread
 *              using the pthreads library. 
 *
 * Author:      Fidility Solutions.
 *
 * Date:        28/02/2024.
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
 *              about the thread IDs and it terminates using pthread_exit() system call.
 *
 * Arguments:   None.
 * 
 * Returns:     NULL: Since the function is of type void *, it returns NULL to indicate successful completion 
 *              of the thread function.
 *
 */

void *threadfunction(void *arg) 
{
	printf("New Thread function started with Thread ID(TID): %ld\n",syscall(SYS_gettid));
	/* Simulating some work done by the thread */

	sleep(5); 
	printf("New Thread function exits using pthread_exit() system call after sleep...\n");

	/* Terminating the thread */
	pthread_exit(NULL);
}
/*
 * Function:    main()
 *
 * Description: This is the entry point of the program. It demonstrates the creation and termination
 *              of a thread using the pthreads library. This function shows creates
 *              a new thread, waits for the new thread to finish its execution, and then exits.
 *              Note:A process is itself a thread.Thus the main thread ID and the process ID will be the same.
 *
 * Arguments:   None
 *
 * Returns:     0 on successful execution, 1 on failure.
 *
 */
int main() 
{
	printf("Entered into main program\n");
	printf("The Process running with(PID):%d\n",getpid());
	/* Variables Declaration */
    	pthread_t thread;
    	int ThreadCreate;
    	/* Create a thread */
    	ThreadCreate = pthread_create(&thread, NULL, threadfunction, NULL);

	/* Check if thread creation was successful */
    	if (ThreadCreate != 0) {
        	fprintf(stderr, "Error creating thread.\n");
        	exit(EXIT_FAILURE);
    	}
	printf("The main thread ID and the process ID will be the same\n");
	 /* Main thread continues execution while the other thread runs */
	printf("This is Main Thread with ID(TID):%ld\n",syscall(SYS_gettid));

    	/* Wait for the created thread to finish */
	/* pthread_join blocks the main thread until the specified thread terminates */
    	if (pthread_join(thread, NULL) != 0) {
        	fprintf(stderr, "Error joining thread.\n");
        	exit(EXIT_FAILURE);
    	}
	/* Main thread exiting */
    	printf("Upon termination of New thread, the main thread exits as well...\n");
    	return 0;
}

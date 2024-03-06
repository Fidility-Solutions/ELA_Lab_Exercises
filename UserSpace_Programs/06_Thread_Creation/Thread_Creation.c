/******************************************************************************
 * File:        Thread_Creation.c
 *
 * Description: This file contains a C program demonstrating the creation and execution of a single thread
 *              using the pthreads library. 
 *
 * Author:      Fidility Solutions.
 *
 * Date:        28/02/2024.
 *
 * Reference    The Linux Programming Interface book

* *******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<sys/syscall.h>

/*
 * Function: 	threadfunction()
 *
 * Description: This function serves as the entry point for a new thread. It prints various details 
 *              about the process and thread IDs, as well as the argument passed to the thread.
 *
 * Arguments:	A pointer to void representing the argument passed to the thread function. In this 
 *            example, it is expected to be a pointer to a string.
 *
 * Returns: 	NULL: Since the function is of type void *, it returns NULL to indicate successful completion 
 *            of the thread function.
 *
 */
void *threadfunction(void *args)
{
	/* Print a message indicating entry into the thread function */
	printf("Entered into the new thread function\n");

	/* Get the process ID (PID) of the current process (main process). */
	pid_t pid=getpid();
	printf("The process is created when the main function starts, so the Process (PID):%d\n",pid);

	/* Print a message explaining the purpose of the syscall(SYS_gettid) call */
	printf("The syscall(SYS_gettid) is a system call that retrieves the thread ID (TID) of the calling thread\n");

	/* Get the thread ID (TID) of the current thread. */
	pid_t thread = syscall(SYS_gettid);
	printf("This is a new thread and thread ID(TID):%d.\n",thread);

	/* print the argument passed from pthread_create() system call. */
	printf("Argument passed to the thread: \"%s\"\n", (char *)args);
	sleep(5);
	return NULL;

}
/*
 * Function:    main()
 *
 * Description: This is the entry point of the program. It demonstrates the creation and execution 
 *              of a new thread using the pthreads library. The main thread prints a message, creates 
 *              a new thread, waits for the new thread to finish its execution, and then exits.
 *
 * Arguments:   None
 *  
 * Returns:     0 on successful execution, 1 on failure.
 *
 */

int main()
{
	printf("Entered into main program \n");

	/* Declare variables */
	pthread_t thread_id;
	char *message = "Hello, from the main thread.";

	 /* Create a new thread */
	printf("A new thread is created using pthread_create() system call \n");
	if (pthread_create(&thread_id, NULL, threadfunction, (void *)message) != 0)
	{

		/* Print error message if thread creation fails */
		fprintf(stderr, "Error creating thread.\n");

		/* Exit with error code */
		return 1;
	}

	/* Wait for the created thread to finish */
	printf("The main thread will wait for termination of new thread.This is done by pthread_join() system call. \n");
	if (pthread_join(thread_id, NULL) != 0)
	{

		/* Print error message if thread creation fails */
		fprintf(stderr, "Error joining thread.\n");

		/* Exit with error code */
		return 1;
	}

	/* main thread is exiting */
	printf("Exiting the Main thread.\n");
	return 0;
}

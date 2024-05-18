/*******************************************************************************
 * File		: Thread_Creation.c
 *
 * Description	: This file contains a C program demonstrating the creation and execution of a single thread
 *                using the pthreads library. 
 *
 * Author	: Fidility Solutions.
 *
 * Date		: 24/02/2024.
 *
 * Reference   	: "The Linux Programming Interface" book.

* *******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/wait.h>
#include<sys/syscall.h>

/*
 * Function	: threadfunction()
 *
 * Description	: This function serves as the entry point for a new thread. It prints various details 
 *                about the process and thread IDs, as well as the argument passed to the thread.
 *
 * Parameters	 : args-A pointer to void representing the argument passed to the thread function. In this 
 *             	   example, it is expected to be a pointer to a string.
 *
 * Returns	: NULL: Since the function is of type void *, it returns NULL to indicate successful completion 
 *            	  of the thread function.
 *
 */
void *threadfunction(void *args)
{
	/* Print a message indicating entry into the thread function */
	printf("Entered into the thread function\n");
	/* Get the process ID (PID) of the current process (main process). */
	pid_t pid=getpid();
	printf("The process is initiated upon the commencement of the main function with the Process (PID):%d\n",pid);
	/* Print a message explaining the purpose of the syscall(SYS_gettid) call */
	printf("The syscall(SYS_gettid) is a system call that retrieves the thread ID (TID) of the current thread\n");
	/* Get the thread ID (TID) of the current thread. */
	pid_t thread = syscall(SYS_gettid);
	printf("The retrieved thread ID(TID):%d.\n",thread);
	/* print the argument passed from pthread_create() system call. */
	printf("Argument passed to the thread: \"%s\"\n", (char *)args);
	return NULL;
}
/*
 * Function:    main()
 *
 * Description: This is the entry point of the program. It demonstrates the creation and execution 
 *              of a new thread using the pthreads library. The main thread prints a message, creates 
 *              a new thread, waits for the new thread to finish its execution, and then exits.
 *
 * Parameters:   None
 *  
 * Returns:     0 on successful execution, 1 on failure.
 *
 */

int main(void)
{
	printf("Entered into main program which is the main thread \n");
	/* Declare variables */
	pthread_t thread_id;
	char *message = "Hello, from the main thread.";
	 /* Create a new thread */
	if (pthread_create(&thread_id, NULL, threadfunction, (void *)message) != 0)
	{
		/* Print error message if thread creation fails */
		fprintf(stderr, "Error creating thread.\n");
		/* Exit with error code */
		return 1;
	}
	/* Wait for the created thread to finish */
	if (pthread_join(thread_id, NULL) != 0)
	{
		/* Print error message if thread creation fails */
		fprintf(stderr, "Error joining thread.\n");
		/* Exit with error code */
		return 1;
	}
	/* main thread is exiting */
	printf("Main thread exiting.\n");
	return 0;
}

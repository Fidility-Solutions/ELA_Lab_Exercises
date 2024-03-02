/******************************************************************************
 * File: 	Thread_Attributes.c
 *
 * Description: This program demonstrates the creation and execution of detached threads with custom thread 
 * 		attributes.It creates multiple detached threads with a specific stack size and detached state 
 * 		using pthread_attr_t.The main thread initializes the thread attributes, creates the detached 
 * 		threads, and then exits without waiting for the threads to complete.
 *
 * Usage: 	./Thread_Attributes
 * 
 * Author: 	Fidility Solutions.
 *  
 * Date:        28/02/2024.
 *
 * Reference    The Linux Programming Interface book
 ******************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include<stdint.h>
#include<unistd.h>
#include<sys/syscall.h>

#define NUM_THREADS 3

/* Structure to hold thread arguments */
struct ThreadArgs {
    int thread_number;
    const char *message;
    size_t size;
};
/*
 * Function: 	threadfunction()
 * --------------------------
 * 
 * Description:	This function is executed by each detached thread. It prints information about the attributes 
 * 		received by the thread and performs specific actions based on the thread numbers.
 * 
 * Parameters:	arg: Pointer to the thread arguments containing the thread ID, message, and size.
 * 
 * Returns:	NULL after completing the thread execution.
 *
 */
void *threadfunction(void *arg) {

	/* structure to get the argumets */
    	struct ThreadArgs *args = (struct ThreadArgs *)arg;

    	/* Printing message and size of message based on thread number */
    	switch (args->thread_number) {
        case 1:
		/* if thread_number is 1 it will execute */
            	printf("Thread %d received message: %s and respective thread id(TID) is %ld\n", args->thread_number, args->message,syscall(SYS_gettid));
            	printf("Size received: %zu bytes\n", args->size);
		sleep(10);
            	break;
        case 2:
		/* if thread_number is 2 it will execute */
            	printf("Thread %d received message: %s and respective thread id(TID) is %ld\n", args->thread_number, args->message,syscall(SYS_gettid));
            	printf("Size received: %zu bytes\n", args->size);
		sleep(10);
            	break;
        case 3:
		/* if thread_number is 3 it will execute */
            	printf("Thread %d received message: %s and respective thread id(TID) is %ld\n", args->thread_number, args->message,syscall(SYS_gettid));
            	printf("Size received: %zu bytes\n", args->size);
		sleep(10);
            	break;
        default:
		/* if thread_number is unknown it will execute */
            	printf("Unknown thread %d with thread id(TID):%ld\n", args->thread_number,syscall(SYS_gettid));
			sleep(10);
		break;
    	}

    	pthread_exit(NULL);
}
/*
 * Function: main()
 * 
 * Description:	Entry point of the program. Initializes thread attributes, creates detached threads with custom 
 * 		attributes, and exits without waiting for the threads to complete.
 *
 * Argumets:	None
 *
 * Returns:	0 upon successful execution of the program.
 */
int main() {
    printf("This program demonstates the use of Thread Attributes \n");
    pthread_t threads[NUM_THREADS];
    pthread_attr_t attr;

    /* 1MB stack size Attribute to thread */
    size_t stack_size = 1024 * 1024; 

    /* Initialize thread attributes */
    printf("The function pthread_attr_init(&attr); is used to initialize a pthread attribute object attr\n");
    pthread_attr_init(&attr);


    /*set the stack size to new thread */
    printf("The pthread_attr_setstacksize(); sets the stack size attribute of the pthread attribute.\n");
    pthread_attr_setstacksize(&attr, stack_size);

    /* set thread as detached */
    printf("Setting thread attribute detach state to detached.\n");
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    /* Create thread arguments and pass to new thread*/
    struct ThreadArgs thread_args[NUM_THREADS] = {
        {1, "Hello from Thread 1", sizeof("Hello from Thread 1")},
        {2, "Greetings from Thread 2", sizeof("Greetings from Thread 2")},
        {3, "Hi there from Thread 3", sizeof("Hi there from Thread 3")}
    };

    /* Creating multiple threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], &attr, threadfunction, (void *)&thread_args[i]) != 0) {
            fprintf(stderr, "Error creating thread %d.\n", i + 1);
            return 1;
        }
    }

    /* Destroy thread attributes */
    printf("Thread attributes can be destroyed using pthread_attr_destroy() system call,it won't effect to the threads \n");
    pthread_attr_destroy(&attr);

    /* Main thread exits without waiting for detached threads to finish */
    printf("Main thread exits without waiting for detached threads to finish with TID:%ld\n",syscall(SYS_gettid));
    pthread_exit(NULL);    
}


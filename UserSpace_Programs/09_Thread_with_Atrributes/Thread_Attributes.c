/******************************************************************************
 * File		: Thread_Attributes.c
 *
 * Description	: This program demonstrates the creation and execution of detached threads with custom thread 
 * 		  attributes.It creates multiple detached threads with a specific stack MsgSize and detached state 
 * 		  using pthread_attr_t.The main thread initializes the thread attributes, creates the detached 
 * 		  threads, and then exits without waiting for the threads to complete.
 *
 * Usage	: ./Thread_Attributes
 * 
 * Author	: Fidility Solutions.
 *  
 * Date		: 26/02/2024.
 *
 * Reference    : "The Linux Programming Interface" book.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>

#define NUM_THREADS 3

/* Structure to hold thread arguments */
struct ThreadArgs{
    uint8_t u8ThreadNum;
    const char *s8Msg;
    size_t MsgSize;
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
    	struct ThreadArgs *StrTArgs = (struct ThreadArgs *)arg;

    	/* Printing message and size of message based on thread number */
    	switch (StrTArgs->u8ThreadNum) {
        case 1:
		/* if u8ThreadNum is 1 it will execute */
            	printf("Thread %d received message %s and its respective thread id(TID) is %ld\n", StrTArgs->u8ThreadNum, StrTArgs->s8Msg,syscall(SYS_gettid));
            	printf("Size received: %lu bytes\n", StrTArgs->MsgSize);
		sleep(10);
            	break;
        case 2:
		/* if u8ThreadNum is 2 it will execute */
            	printf("Thread %d received message %s and its respective thread id(TID) is %ld\n", StrTArgs->u8ThreadNum,StrTArgs->s8Msg,syscall(SYS_gettid));
            	printf("Size received: %lu bytes\n", StrTArgs->MsgSize);
		sleep(10);
            	break;
        case 3:
		/* if u8ThreadNum is 3 it will execute */
            	printf("Thread %d received message %s and its respective thread id(TID) is %ld\n", StrTArgs->u8ThreadNum,StrTArgs->s8Msg,syscall(SYS_gettid));
            	printf("Size received: %lu bytes\n", StrTArgs->MsgSize);
		sleep(10);
            	break;
        default:
		/* if u8ThreadNum is unknown it will execute */
            	printf("Unknown thread %d with thread id(TID):%ld\n", StrTArgs->u8ThreadNum,syscall(SYS_gettid));
			sleep(10);
		break;
    	}

    	pthread_exit(NULL);
}
/*
 * Function	: main()
 * 
 * Description	: Entry point of the program. Initializes thread attributes, creates detached threads with custom 
 * 		  attributes, and exits without waiting for the threads to complete.
 *
 * Parameters	: None
 *
 * Returns	: 0 upon successful execution of the program.
 */
int main(void) {
    printf("This program demonstrates the use of Thread Attributes \n");
    pthread_t Threads[NUM_THREADS];
    pthread_attr_t ThreadAttr;

    /* 1MB stack MsgSize Attribute to thread */
    size_t stack_size = 1024 * 1024; 

    /* Initialize thread attributes */
    printf("The function pthread_attr_init(&ThreadAttr) is used to initialize a pthread attribute object attr\n");
    pthread_attr_init(&ThreadAttr);


    /*set the stack MsgSize to new thread */
    printf("The pthread_attr_setstacksize() sets the stack size attribute of the pthread attribute.\n");
    pthread_attr_setstacksize(&ThreadAttr, stack_size);

    /* set thread as detached */
    printf("Assigning the detach state of the thread attribute to detached.\n");
    pthread_attr_setdetachstate(&ThreadAttr, PTHREAD_CREATE_DETACHED);

    /* Create thread arguments and pass to new thread*/
    struct ThreadArgs Thread_StrTArgs[NUM_THREADS] = {
        {1, "Hello", sizeof("Hello from Thread 1")},
        {2, "Greetings",sizeof("Greetings from Thread 2")},
        {3, "Hi there", sizeof("Hi there from Thread 3")}
    };

    /* Creating multiple threads */
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&Threads[i], &ThreadAttr, threadfunction, (void *)&Thread_StrTArgs[i]) != 0) {
            fprintf(stderr, "Error creating thread %d.\n", i + 1);
            return 1;
        }
    }

    /* Destroy thread attributes */
    printf("Thread attributes can be destroyed using pthread_attr_destroy() system call without affecting the threads.\n");
    pthread_attr_destroy(&ThreadAttr);

    /* Main thread exits without waiting for detached threads to finish */
    printf("Main thread with TID:%ld exited without waiting for other threads to finish\n",syscall(SYS_gettid));
    pthread_exit(NULL);    
}


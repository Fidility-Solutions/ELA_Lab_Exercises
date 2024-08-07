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
	sleep(1);
    	/* Printing message and size of message based on thread number */
    	switch (StrTArgs->u8ThreadNum) {
        case 1:
		/* if u8ThreadNum is 1 it will execute */
            	printf("\nThread_%d received message \"%s\" and its respective thread id(TID) is %ld\n", StrTArgs->u8ThreadNum, StrTArgs->s8Msg,syscall(SYS_gettid));
            	printf("Size received: %lu bytes\n", StrTArgs->MsgSize);
		sleep(10);
            	break;
        case 2:
		/* if u8ThreadNum is 2 it will execute */
            	printf("\nThread_%d received message \"%s\" and its respective thread id(TID) is %ld\n", StrTArgs->u8ThreadNum,StrTArgs->s8Msg,syscall(SYS_gettid));
            	printf("Size received: %lu bytes\n", StrTArgs->MsgSize);
		sleep(10);
            	break;
        case 3:
		/* if u8ThreadNum is 3 it will execute */
            	printf("\nThread_%d received message \"%s\" and its respective thread id(TID) is %ld\n", StrTArgs->u8ThreadNum,StrTArgs->s8Msg,syscall(SYS_gettid));
            	printf("Size received: %lu bytes\n", StrTArgs->MsgSize);
		sleep(10);
            	break;
        default:
		/* if u8ThreadNum is unknown it will execute */
            	printf("\nUnknown thread %d with thread id(TID):%ld\n", StrTArgs->u8ThreadNum,syscall(SYS_gettid));
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
   	 pthread_attr_init(&ThreadAttr);
   	 printf("\nThread attributes initialized\n");

   	 /*set the stack MsgSize to new thread */
   	 pthread_attr_setstacksize(&ThreadAttr, stack_size);
   	 printf("\nSets the stack size to thread attribute.\n");

   	 /* set thread as detached */
   	 pthread_attr_setdetachstate(&ThreadAttr, PTHREAD_CREATE_DETACHED);
   	 printf("\nAssigned the detach state to thread attribute\n");

   	 /* Create thread arguments and pass to new thread*/
   	 struct ThreadArgs Thread_StrTArgs[NUM_THREADS] = {
   	     {1, "Hello", sizeof("Hello")},
   	     {2, "Greetings",sizeof("Greetings")},
   	     {3, "Hi there", sizeof("Hi there")}
   	 };

   	 /* Creating multiple threads */
   	 for (int i = 0; i < NUM_THREADS; i++) {
   	     if (pthread_create(&Threads[i], &ThreadAttr, threadfunction, (void *)&Thread_StrTArgs[i]) != 0) {
   	         fprintf(stderr, "Error creating thread %d.\n", i + 1);
   	         return 1;
   	     }
   	 }

   	 /* Destroy thread attributes */
   	 pthread_attr_destroy(&ThreadAttr);
   	 printf("\nThread attributes destroyed\n");
   	 /* Main thread exits without waiting for detached threads to finish */
   	 printf("Main thread with TID:%ld exited and now threads are in detached state\n",syscall(SYS_gettid));
   	 pthread_exit(NULL);    
}


/*
 * File		: Dynamic_Condition_Variable.c
 *
 * Description	: This program demonstrates the dynamic initialization of condition variables and mutexes 
 * 		  for inter-process communication. It creates two threads that operate on a shared data 
 * 		  structure while holding a mutex lock. The threads increment a shared variable and signal 
 * 		  the condition variable to wake up waiting threads. The main thread waits for the condition 
 * 		  variable to be signaled and prints the updated value of the shared variable after each thread 
 * 		  has finished its operation.
 *
 * Usage	: ./Dynamic_Condition_Variable
 *
 * Author:	  Fidility Solutions
 *
 * Date		: 10/03/2024
 *
 * Reference	: The "Linux Programming Interface" book.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <unistd.h>
/* 
 * Function:    errExit
 * ------------------
 * Description: Prints the error message corresponding to the given system error number using perror(), 
 *              and exits the program.
 *
 * Parameters:  message - The error message to be printed.
 *
 * Returns:     None
 */
void errExit(const char *ps8Msg){
	perror(ps8Msg);
	exit(EXIT_FAILURE);
}
void errMsg(const char *msg,pid_t Tid){
	fprintf(stderr, "%s: %d\n", msg, Tid);
	exit(EXIT_FAILURE);
}

/*Structure to hold data shared between threads */
struct SharedData {
    	int value;
    	/* Pointer to dynamically allocated mutex */
    	pthread_mutex_t* mutex;   
 	/* Pointer to dynamically allocated condition variable */				
    	pthread_cond_t* cond;   
};

/* Function executed by threads */
/*
 * Function   : ThreadFunc
 *
 * Description: This function is executed by threads. It increments a shared variable by 100 
 * 		while holding a mutex lock. It then signals the condition variable to wake up 
 * 		waiting threads. After completing its operation, it unlocks the mutex.
 *
 * Parameters :
 *     		arg: Argument passed to the thread (struct SharedData pointer)
 *
 * Return     : None
 */
void* ThreadFunc(void* arg){
    	struct SharedData* strSharedData = (struct SharedData*)arg;
    	int state;
	pid_t ThreadId = syscall(SYS_gettid);
    	/* Lock the mutex */
    	state = pthread_mutex_lock(strSharedData->mutex);
	if(state !=0)
		errMsg("Thread_%d failed to lock the mutex",ThreadId);
	printf("\nThread_%d: locked mutex\n",ThreadId);

    	strSharedData->value = strSharedData->value+100;
    	printf("Thread_%d: Incremeted the value of variable to  %d\n",ThreadId, strSharedData->value);
   	/* Signal the condition variable to wake up waiting threads */
        printf("Thread_%d sent condition signal\n",ThreadId);
        if(pthread_cond_signal(strSharedData->cond) !=0)
                errExit("signal condition failed");
	/* Unlock the mutex */
	sleep(1);
        state = pthread_mutex_unlock(strSharedData->mutex);
        if(state != 0)
                errMsg("mutex unlocked failed from Thread %d",ThreadId);
	printf("Thread_%d Mutex is released(unlocked)\n",ThreadId);

    return NULL;
}
/*
 * Function   : main
 *
 * Description: This is the entry point of the program. It initializes shared data, creates threads, 
 * 		and waits for the condition variable to be signaled by the threads. After receiving 
 * 		the signal, it prints the updated value of the shared variable.
 *
 * Parameters : None
 * Return     : 0 on Successful.
 */
int main(void){
	printf("Welcome to Dynamically initalizing condition variable and mutex in Inter-process communication\n");
	int state;
    	/* Initialize shared data */
    	struct SharedData strSharedData;
    	strSharedData.value = 0;

    	/* Dynamically allocate memory for mutex */
    	strSharedData.mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));

    	/* Dynamically allocate memory for condition variable */
    	strSharedData.cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    	if(strSharedData.cond == NULL || strSharedData.mutex == NULL)
        	errExit("malloc failed");
	printf("Memory allocated dynamically to mutex & condition variable\n");
    	/* Initialize mutex */
    	if(pthread_mutex_init(strSharedData.mutex, NULL) != 0) 
        	errExit("pthread_mutex_init failed");

    	/* Initialize condition variable */
    	if(pthread_cond_init(strSharedData.cond, NULL) != 0)
       	 	errExit("pthread_cond_init failed");
	printf("Mutex & condition variable is intialized\n");
    	/* Create threads */
    	pthread_t thread1, thread2;
	if(pthread_create(&thread1, NULL, ThreadFunc, &strSharedData) != 0 ||
                pthread_create(&thread2, NULL, ThreadFunc, &strSharedData) != 0)
                errExit("pthread_create failed");
    	/* Lock the mutex */
    	printf("\nMain thread locked the mutex\n");
    	if(pthread_mutex_lock(strSharedData.mutex) !=0)
	    	errExit("Mutex lock error from main thread");
	
    	/* Wait for the condition variable to be signaled */
    	printf("Main thread released mutex due to the wait condition \n");
    	pthread_cond_wait(strSharedData.cond, strSharedData.mutex);
	printf("\nMain thread aquired lock after receiving cond signal\n");
    	printf("Value after operation from thread is %d\n",strSharedData.value);
	/* Again wait to get signal from another thread */
	printf("Main thread released mutex due to the 2nd wait condition \n");
	pthread_cond_wait(strSharedData.cond, strSharedData.mutex);
	printf("\nMain thread aquired lock after receiving 2nd cond signal\n");
	printf("Value after operation from thread is %d\n",strSharedData.value);
    	/* Terminate the threads */
    	if(pthread_join(thread1, NULL) !=0 || pthread_join(thread2, NULL) !=0)
	    	errExit("Thread termination failed");
	
    	/* Unlock the mutex from main thread */
	if(pthread_mutex_unlock(strSharedData.mutex) !=0)
            	errExit("mutex unlock from main thread failed");
    	printf("Main thread released(unlocked)\n");

    	/* Destroy mutex and condition variable */
    	if(pthread_mutex_destroy(strSharedData.mutex) !=0)
	    	errExit("Failed to destroy mutex");
    
    	if(pthread_cond_destroy(strSharedData.cond) !=0)
	    	errExit("condition variable destroy failed");
    	printf("Condition variable and mutex destroyed\n");

    	/* Free memory */
    	free(strSharedData.mutex);
    	free(strSharedData.cond);
	printf("\nAll threads got terminated\n");
    	return 0;
}


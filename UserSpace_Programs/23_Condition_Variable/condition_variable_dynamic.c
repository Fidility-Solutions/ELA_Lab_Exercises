/*
 * File		: pthread_mutex_cond_dynamic.c
 *
 * Description	: This program demonstrates the dynamic initialization of condition variables and mutexes 
 * 		  for inter-process communication. It creates two threads that operate on a shared data 
 * 		  structure while holding a mutex lock. The threads increment a shared variable and signal 
 * 		  the condition variable to wake up waiting threads. The main thread waits for the condition 
 * 		  variable to be signaled and prints the updated value of the shared variable after each thread 
 * 		  has finished its operation.
 *
 * Usage	: ./pthread_mutex_cond_dynamic 		  
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
	sleep(1);
    	state = pthread_mutex_lock(strSharedData->mutex);
	if(state !=0)
		errMsg("Aquiring mutex lock failed for Thread %d",ThreadId);
	printf("\nThread %d: locked mutex\n",ThreadId);

    	strSharedData->value = strSharedData->value+100;
    	printf("Thread %d: Incremeted the value of variable to : %d\n",ThreadId, strSharedData->value);

	/* Here sleep is to complete thread operation */
	sleep(1);
	/* Signal the condition variable to wake up waiting threads */
        printf("Thread %d sent signal to wake up waiting thread\n",ThreadId);
        if(pthread_cond_signal(strSharedData->cond) !=0)
                errExit("signal condition failed");
	/* Unlock the mutex */
        state = pthread_mutex_unlock(strSharedData->mutex);
        printf("Thread %d Mutex is released(unlocked)\n",ThreadId);
        if(state != 0)
                errMsg("mutex unlocked failed from Thread %d",ThreadId);



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
	printf("Welcome to Dynaically initalized condition variable and mutex in Inter-process communication\n");
	int state;
    	/* Initialize shared data */
    	struct SharedData strSharedData;
    	strSharedData.value = 0;

    	/* Dynamically allocate memory for mutex */
    	printf("Dynamically allocating memory to mutex & condition variable\n");
    	strSharedData.mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    	/* Dynamically allocate memory for condition variable */

    	strSharedData.cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    	if(strSharedData.cond == NULL || strSharedData.mutex == NULL)
        	errExit("malloc failed");

    	/* Initialize mutex */
    	printf("Mutex is intialized\n");
    	if(pthread_mutex_init(strSharedData.mutex, NULL) != 0) 
        	errExit("pthread_mutex_init failed");

    	/* Initialize condition variable */
    	if(pthread_cond_init(strSharedData.cond, NULL) != 0)
       	 	errExit("pthread_cond_init failed");

    	/* Create threads */
    	pthread_t thread1, thread2;
    	if(pthread_create(&thread1, NULL, ThreadFunc, &strSharedData) != 0 ||
        	pthread_create(&thread2, NULL, ThreadFunc, &strSharedData) != 0)
        	errExit("pthread_create failed");

    	/* Lock the mutex */
    	printf("\nMutex locked from Main Thread\n");
    	if(pthread_mutex_lock(strSharedData.mutex) !=0)
	    	errExit("mutex lock error from main thread");
	
    	/* Wait for the condition variable to be signaled */
    	printf("The mutex lock is released from main thread when the cond_wait sys call executed\n");
    	pthread_cond_wait(strSharedData.cond, strSharedData.mutex);

    	printf("\nThe value after operation from thread1 is :%d\n",strSharedData.value);
	/* Again wait to get signal from another thread */
	pthread_cond_wait(strSharedData.cond, strSharedData.mutex);
	printf("\nThe value after operation from thread2 is :%d\n",strSharedData.value);
    	/* Terminate the threads */
    	if(pthread_join(thread1, NULL) !=0|| pthread_join(thread2, NULL) !=0)
	    	errExit("Thread termination failed");
    	printf("\nThreads Terminared\n");
	
    	/* Unlock the mutex from main thread */
	if(pthread_mutex_unlock(strSharedData.mutex) !=0)
            	errExit("mutex unlock from main thread failed");
    	printf("Mutex is released(unlocked) from main thread\n");

    	/* Destroy mutex and condition variable */
    	if(pthread_mutex_destroy(strSharedData.mutex) !=0)
	    	errExit("Mutex destroy failed");
    
    	if(pthread_cond_destroy(strSharedData.cond) !=0)
	    	errExit("condition variable destroy failed");
    	printf("Condition variable and mutex destroyed\n");

    	/* Free memory */
    	free(strSharedData.mutex);
    	free(strSharedData.cond);

    	return 0;
}


/*
 * File name: pthread_mutex_dynamic.c
 *
 * Description: This program demonstrates thread synchronization using mutex (Dynamic Initialization).
 *            	It creates multiple threads that attempt to acquire a mutex lock using different methods:
 *           	one thread tries to acquire the lock using pthread_mutex_trylock(), and another thread
 *            	tries to acquire the lock using pthread_mutex_timedlock(). It also dynamically initializes
 *            	the mutex and destroys it when it is no longer needed.
 *
 * Usage: 	./pthread_mutex_dynamic
 *
 * Author: 	Fidility Solutions
 *
 * Date: 	10/03/2024
 *
 * Reference: The "Linux Programming Interface" book
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define NUM_THREADS 2

pthread_mutex_t mutex;
/*
 * Function   : ThreadFunc
 *
 * Description: Thread function that increments a global counter while holding a mutex lock.
 *
 * Parameters :
 *   	        arg: Argument passed to the thread (thread number)
 *
 *return      : None
 */
void* ThreadFun(void* arg) {
	long ThreadNum = (long)arg;

    	if(ThreadNum == 1){
        	/* Thread 1: Attempt to lock mutex using pthread_mutex_trylock() */
        	printf("Thread %ld trying to acquire mutex using try lock...\n", ThreadNum);
		sleep(1);
        	int8_t s8Res = pthread_mutex_trylock(&mutex);
        	if(s8Res == 0){
            		printf("Thread %ld acquired mutex using try lock.\n", ThreadNum);
            		/* Simulate some work */
		
            		pthread_mutex_unlock(&mutex);
            		printf("Thread %ld released mutex.\n", ThreadNum);
        	} 
		else{
            		printf("Thread %ld failed to acquire mutex using try lock because mutex is locked with others. Error code: %d\n", ThreadNum, s8Res);
        	}
    	}
    	if(ThreadNum == 2){
        	/* Thread 2: Attempt to lock mutex using pthread_mutex_timedlock() */
        	printf("Thread %ld trying to acquire mutex using timed lock...\n", ThreadNum);
        	struct timespec timeout;
        	clock_gettime(CLOCK_REALTIME, &timeout);
		/* Set timeout to 2 seconds */
        	timeout.tv_sec += 2; 
		sleep(1);

        	int8_t s8Res = pthread_mutex_timedlock(&mutex, &timeout);
        	if(s8Res == 0){
            		printf("Thread %ld acquired mutex using timed lock.\n", ThreadNum);
            		/* Simulate some work */
            		pthread_mutex_unlock(&mutex);
            		printf("Thread %ld released mutex.\n", ThreadNum);
        	}
		else{
            		printf("Thread %ld failed to acquire mutex using timed lock within the time because another thread locked the mutex. Error code: %d\n", ThreadNum, s8Res);
        	}
    	}
    return NULL;
}
/*
 * Function   : main
 *
 * Description: Entry point of the program. Creates multiple threads to execute the ThreadFunc function,
 *            	which increments a global counter while holding a mutex lock for synchronization.
 *
 * Parameters : None
 *
 * Return     : 0 on Successful.
*/

int main(void){
	printf("Welcome to Dynamic intializing mutual exclusion(mutex)\n");
	/*Declare variables */
    	pthread_t Threads[NUM_THREADS];
    	int8_t s8Res;
    	long i;

    /* Initialize dynamic mutex */
	pthread_mutex_t *mutex=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	if(mutex == NULL){
		fprintf(stderr,"failed to allocate memory for mutex\n");
		exit(EXIT_FAILURE);
	}
	printf("Dynamic mutex is intialized\n");

	/*Initialize the mutex attribute */
    	pthread_mutexattr_t mutexattr;
    	s8Res = pthread_mutexattr_init(&mutexattr);
    	if(s8Res != 0){
        	fprintf(stderr, "Error: Failed to initialize mutex attribute. Error code: %d\n", s8Res);
        	exit(EXIT_FAILURE);
    	}
	printf("The Mutex attribute is intialized\n");

	/* Set the type of operation */
    	s8Res = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_NORMAL);
    	if(s8Res != 0){
        	fprintf(stderr, "Error: Failed to set mutex attribute type. Error code: %d\n", s8Res);
        	exit(EXIT_FAILURE);
    	}
	
	/* initialize the mutex with atrribute */
    	s8Res = pthread_mutex_init(mutex, &mutexattr);
    	if(s8Res != 0){
        	fprintf(stderr, "Error: Failed to initialize mutex. Error code: %d\n", s8Res);
        	exit(EXIT_FAILURE);
    	}
	printf("The mutex is initialized with attribute(NOMAL)\n");

    	/* Create Threads */
    	for(i = 0; i < NUM_THREADS; i++){
        	s8Res = pthread_create(&Threads[i], NULL, ThreadFun, (void *)i+1);
        	if(s8Res != 0){
            		fprintf(stderr, "Error: Failed to create thread %ld. Error code: %d\n", i+1, s8Res);
            		exit(EXIT_FAILURE);
        	}
    	}

    	/* Join Threads */
    	for(i = 0; i < NUM_THREADS; i++){
        	s8Res = pthread_join(Threads[i], NULL);
        	if(s8Res != 0){
            		fprintf(stderr, "Error: Failed to join thread %ld. Error code: %d\n", i+1, s8Res);
            		exit(EXIT_FAILURE);
        	}
    	}

    	/* Destroy mutex when no longer needed */
	printf("Destroying mutex...\n");
    	s8Res = pthread_mutex_destroy(mutex);
    	if(s8Res != 0){
        	fprintf(stderr, "Error: Failed to destroy mutex. Error code: %d\n", s8Res);
        	exit(EXIT_FAILURE);
    	}

    return 0;
}


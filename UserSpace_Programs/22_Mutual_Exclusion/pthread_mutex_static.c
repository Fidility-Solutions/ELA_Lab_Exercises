/*
 * File name: 	pthread_mutex_static.c
 *
 * Description: This program demonstrates thread synchronization using mutex (Static Initialization). 
 *            	It creates multiple threads that increment a global counter while holding a mutex lock
 *            	to ensure mutual exclusion. Each thread locks the mutex, increments the counter several times,
 *            	and then unlocks the mutex. The program utilizes static initialization of the mutex.
 *
 * Author: 	Fidility Solutions
 *
 * Date: 	10/03/2024
 *
 * Reference: 	The "Linux Programming Interface" book.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syscall.h>

#define NUM_INCREMENTS 50000
#define NUM_THREADS 2
#define MUTEX_LOCK_ENABLE 1
/* Static initialization of the mutex */
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

/*
 * Function: 	ThreadFunc
 *
 * Description: Thread function that increments a global counter while holding a mutex lock.
 *            	Each thread locks the mutex, increments the counter several times, and then unlocks the mutex.
 * Parameters:
 * 		arg: Argument passed to the thread (thread number)
 *
 * Return: 	None
 *
*/
void *ThreadFunc(void *arg){
	long ThreadNum = (long)arg;
    	int8_t s8Check;

    	/* Lock the mutex before modifying the counter */
#if MUTEX_LOCK_ENABLE
    	pthread_mutex_lock(&mutex);
    	printf("\nThe mutex is locked for Thread%ld and The respective thread id(TID):%ld\n\n", ThreadNum, syscall(SYS_gettid));
#endif
    	for(int i = 0; i < NUM_INCREMENTS; i++){
        	counter++;
        	printf("The counter value incremented by: %d\n", counter);
    	}
    	/* Unlock the mutex after modifying the counter */
#if MUTEX_LOCK_ENABLE
    	pthread_mutex_unlock(&mutex);
    	printf("Mutex is unlocked from thread%ld\n", ThreadNum);
#endif
    	printf("Thread%ld finished operation. The counter value = %d\n", ThreadNum, counter);
    	pthread_exit(NULL);
}

/*
 * Function: 	main
 *
 * Description: Entry point of the program. Creates multiple threads to execute the ThreadFunc function,
 *            	which increments a global counter while holding a mutex lock for synchronization.
 *
 * Parameters: 	None
 *
 * Return: 0 on Successful.
 *
 */
int main(void){
    	printf("Welcome to Thread synchronization using mutex (Static Initialization)\n");
    	pthread_t threads[NUM_THREADS];
    	int8_t s8Status;

    	/* Creating threads */
    	for(long t = 0; t < NUM_THREADS; t++){
        	s8Status = pthread_create(&threads[t], NULL, ThreadFunc, (void *)t);
        	if(s8Status){
            		printf("Error: Unable to create thread %ld\n", t);
            		exit(-1);
        	}
    	}

    	/* Join threads */
    	for(int t = 0; t < NUM_THREADS; t++){
        	pthread_join(threads[t], NULL);
    	}
    	return 0;
}


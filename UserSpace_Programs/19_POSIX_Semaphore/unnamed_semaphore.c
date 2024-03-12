/******************************************************************************
 * File:        unnamed_semaphore.c
 *
 * Description: This program demonstrates the usage of POSIX unnamed semaphores for thread synchronization. 
 *              It creates a semaphore with an initial value of 1, spawns multiple threads, and each thread 
 *              increments a shared variable after acquiring the semaphore.
 *
 * Usage:       ./unnamed_semaphore
 * 
 * Author:      Fidility Solutions
 *  
 * Date:        6/03/2024
 *
 * Reference:   The Linux Programming Interface book
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_THREADS 3

/* Global declaration */
int SharedResource =0;
static sem_t semaphore;

/* 
 * Function: 	errExit
 * ------------------
 * Description: Prints the error message corresponding to the given system error number using perror(), 
 * 		and exits the program.
 *
 * Parameters: 	message - The error message to be printed.
 *
 * Returns: 	None
 */
void errExit(const char *message){
	perror(message);
	exit(EXIT_FAILURE);
}
/* 
 * Function: 	threadfn
 * -------------------
 * Description: Represents the critical section accessed by multiple threads. Each thread increments a shared variable
 *              after acquiring the semaphore.
 *
 * Parameters: 	arg - Pointer to the argument passed to the thread function
 *
 * Returns: 	None
 */
void *theadfn(void *arg){
	/*Thread number */
	int ThreadNum=*((int8_t *)arg);

	sleep(2);

	/*Acquire the semaphore (wait) */
	printf("Therad%d waiting for semaphore...\n",ThreadNum);
	if(sem_wait(&semaphore) == -1)
		errExit("sem_wait error");

	printf("Thread%d acquired semaphore\n",ThreadNum);
	/* Increment the shared resource value */
	SharedResource++;
	printf("Thread%d incremented shared resource to: %d\n",ThreadNum,SharedResource);

	/*Release the semaphore(post) */
	if(sem_post(&semaphore)==-1)
		errExit("sem_post error");
	printf("Therad%d released the semaphore and exiting...\n",ThreadNum);
	pthread_exit(NULL);
}
/* 
 * Function: 	main
 * ---------------
 * Description: The main function of the program. It initializes the unnamed semaphore, spawns multiple threads,
 *              and waits for them to finish their work.
 *
 * Parameters: 	None
 *
 * Returns:    	0 upon successful execution of the program
 */
int main(void){
	printf("Welcome to POSIX unnamed semaphore\n");
	pthread_t Threads[NUM_THREADS];
	int as8ThreadArgs[NUM_THREADS];

	/* Initialize semaphore with an initial value of 1 */
	printf("The semaphore is initialized with value 1\n");
	if(sem_init(&semaphore,0,1)==-1)
		errExit("sem_init");

	/* Create threads */
	for(uint8_t i=0;i<NUM_THREADS;i++){
		as8ThreadArgs[i]=i;
		if(pthread_create(&Threads[i],NULL,theadfn,&as8ThreadArgs[i])!=0)
			errExit("pthread_create error");
	}
	/* Wait for thread to join */
	for(int i=0;i<NUM_THREADS;i++){
		if(pthread_join(Threads[i],NULL)!=0)
			errExit("pthread_join error");
	}
	/*Destroy the semaphore */
	if(sem_destroy(&semaphore) == -1)
		errExit("sem_destroy error");
	return 0;
}




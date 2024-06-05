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

#define NUM_THREADS 2
#define NUM_ITER 5

/* Global declaration */
int SharedResource =0;
static sem_t semaphore;

/* 
 * Function	: errExit
 * ------------------
 * Description	: Prints the error message corresponding to the given system error number using perror(), 
 * 		  and exits the program.
 *
 * Parameters	: message - The error message to be printed.
 *
 * Returns	: None
 */
void errExit(const char *message){
	perror(message);
	exit(EXIT_FAILURE);
}
/* 
 * Function	: threadfn
 * -------------------
 * Description	: Represents the critical section accessed by multiple threads. Each thread increments a shared variable
 *              after acquiring the semaphore.
 *
 * Parameters	: arg - Pointer to the argument passed to the thread function
 *
 * Returns	: None
 */
void *theadfn(void *arg){
	/*Thread number */
	int ThreadNum=*((int8_t *)arg);
	int i=0;

	/*Acquire the semaphore (wait) */
	if(ThreadNum == 0)
	{
		for(i=0;i<NUM_ITER;i++)
		{
			printf("Thread%d waiting for semaphore...\n",ThreadNum);
			if(sem_wait(&semaphore) == -1)
				errExit("sem_wait error");

			printf("Thread%d got semaphore\n",ThreadNum);
			/* Increment the shared resource value */
			SharedResource++;
			printf("Thread%d incremented shared resource to: 	 %d\n",ThreadNum,SharedResource);
		}
		pthread_exit(NULL);
	}
	else if(ThreadNum == 1)
	{
		for(i=0;i<NUM_ITER;i++)
		{
			/*Post the semaphore(post) */
			sleep(2);
			if(sem_post(&semaphore)==-1)
				errExit("sem_post error");
			printf("Thread%d posted the semaphore ...\n",ThreadNum);
		}
		pthread_exit(NULL);
		}
}
/* 
 * Function	: main
 * ---------------
 * Description	: The main function of the program. It initializes the unnamed semaphore, spawns multiple threads,
 *                and waits for them to finish their work.
 *
 * Parameters	: None
 *
 * Returns	: 0 upon successful execution of the program
 */
int main(void){
	printf("Welcome to POSIX unnamed semaphore\n");
	pthread_t Threads[NUM_THREADS];
	int as8ThreadArgs[NUM_THREADS];

	/* Initialize semaphore with an initial value of 0 */
	printf("The semaphore is initialized with value 0\n");
	if(sem_init(&semaphore,0,0)==-1)
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
	printf("The semaphore is destroyed\n");
	return 0;
}




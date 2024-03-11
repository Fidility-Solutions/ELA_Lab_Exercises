#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <pthread.h>

#define NUM_THREADS 5

/* Global declaration */
int8_t s8SharedResource =0;
static sem_t semaphore;

/* Error function to notify error and exit */
void errExit(const char *message){
	perror(message);
	exit(EXIT_FAILURE);
}
void *theadfn(void *arg){
	/*Thread number */
	int8_t s8ThreadNum=*((int8_t *)arg);

	sleep(2);

	/*acquire the semaphore (wait) */
	printf("Therad%d waiting for semaphore...\n",s8ThreadNum);
	if(sem_wait(&semaphore) == -1)
		errExit("sem_wait error");

	printf("Thread%d acuired semaphore\n",s8ThreadNum);
	/* Increment the shared resource value */
	s8SharedResource++;
	printf("Thread%d incremented shared resource to: %d\n",s8ThreadNum,s8SharedResource);

	/*release the semaphore(post) */
	if(sem_post(&semaphore)==-1)
		errExit("sem_post error");
	printf("Therad%d released the semaphore and exiting...\n",s8ThreadNum);
	pthread_exit(NULL);
}
int main(void){
	printf("Welcome to POSIX unnamed semaphore\n");
	pthread_t Threads[NUM_THREADS];
	int as8ThreadArgs[NUM_THREADS];

	/* initialize semaphore with an initial value of 1 */
	printf("The semaphore is initialized with value 1\n");
	if(sem_init(&semaphore,0,1)==-1)
		errExit("sem_init");

	/* create threads */
	for(uint8_t i=0;i<NUM_THREADS;i++){
		as8ThreadArgs[i]=i;
		if(pthread_create(&Threads[i],NULL,theadfn,&as8ThreadArgs[i])!=0)
			errExit("pthread_create error");
	}
	/* wait for thread to join */
	for(int i=0;i<NUM_THREADS;i++){
		if(pthread_join(Threads[i],NULL)!=0)
			errExit("pthread_join error");
	}
	/*destroy the semaphore */
	if(sem_destroy(&semaphore) == -1)
		errExit("sem_destroy error");
	return 0;
}




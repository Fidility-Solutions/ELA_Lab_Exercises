/******************************************************************************
 * File:        named_semaphore_using_threads.c
 *
 * Description: This program demonstrates the usage of POSIX named semaphores for thread synchronization. 
 *              It creates a named semaphore, spawns multiple threads, and each thread increments a shared variable
 *              after acquiring the semaphore.
 *
 * Usage:       ./named_semaphore_using_threads
 * 
 * Author:      Fidility Solutions
 *  
 * Date:        6/03/2024
 *
 * Reference:   The Linux Programming Interface
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>

#define NUM_THREADS 6
#define SEM_NAME "/Named_Semaphore"

/* Error notify and exit function */
void errExit(const char *ps8Msg) {
    perror(ps8Msg);
    exit(EXIT_FAILURE);
}

sem_t *Semaphore;

static int8_t s8Var=0;

/* 
 * Function	: threadfn
 * -------------------
 * Description	: Represents the critical section accessed by multiple threads. Each thread increments a shared variable
 *              after acquiring the semaphore.
 *
 * Parameters	: args - Pointer to the argument passed to the thread function
 *
 * Returns	: None
 */
void *threadfn(void *args){
	int s8ThreadNum = *((int*)args);
	/*wait for access to the critical section(shared resources) */
	printf("Thread%d waiting of semaphore...\n",s8ThreadNum);
	if(sem_wait(Semaphore) == -1)
		errExit("sem_wait failed");
	s8Var=s8Var+10;
	/*Access the critical section(shared resources) */
	printf("Thread%d acuired semaphore, the shared resource value after increment:%d\n",s8ThreadNum,s8Var);

	/*Release access to the critical section(shared resources) */
	sem_post(Semaphore);
	printf("Thread%d released semaphore and exiting...\n",s8ThreadNum);
	pthread_exit(NULL);
}
/* 
 * Function	: main
 * ---------------
 * Description	: The main function of the program. It initializes the named semaphore, spawns multiple threads,
 *              and waits for them to finish their work.
 *
 * Parameters	: None
 *
 * Returns	: 0 upon successful execution of the program
 */
int main(void){
	printf("Welcome to POSIX named semaphore explaing with Thread concept\n");
	pthread_t Thread[NUM_THREADS];
	int8_t au8ThreadArgs[NUM_THREADS];
	int8_t s8Thread;
	/* Create/Open the named semaphore */
	printf("Semaphore is created/opened\n");
	Semaphore = sem_open(SEM_NAME,O_CREAT|O_EXCL,0644,1);
	if(Semaphore == SEM_FAILED)
		errExit("sem-open fail");
	/* create therads */
	for(int i =0;i<NUM_THREADS;i++){
		sleep(1);
		s8Thread=pthread_create(&Thread[i],NULL,threadfn,(void *)&i);
		if(s8Thread!=0){
			printf("ERROR:return code from pthread_create is %d\n",s8Thread);
			errExit("thread_create error");
		}
	}
	/* wait for thread to finish work */
	for(int i=0;i<NUM_THREADS;i++)
		pthread_join(Thread[i],NULL);

	/* close the named semaphore */
	if(sem_close(Semaphore) == -1)
		errExit("sem_close error");
	printf("Semaphore closed\n");
	/*remove named semaphore */
	if(sem_unlink(SEM_NAME) == -1)
		errExit("sem_unlink fail");
	printf("Semaphore deleted\n");
	printf("Exiting...\n");
	return 0;
}



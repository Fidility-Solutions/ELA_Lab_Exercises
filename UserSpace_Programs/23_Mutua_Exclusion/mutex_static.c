#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syscall.h>

#define NUM_INCREMENTS 	5
#define NUM_THREADS 	2

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void errExit(const char *ps8Msg){
	perror(ps8Msg);
	exit(EXIT_FAILURE);
}

int counter = 0;
void *ThreadFunc(void *arg){
	long ThreadNum =(long)arg;
	int8_t s8Check;

	/* Lock the mutex before modifying the counter */
        pthread_mutex_lock(&mutex);
	 printf("\nThe mutex is locked for Thread%ld and The respective thread id(TID):%ld\n\n",ThreadNum,syscall(SYS_gettid));
	for(int i = 0; i < NUM_INCREMENTS; i++){
        counter++;
	printf("The counter value incremeted by:%d\n",counter);

    }
	/* Unlock the mutex after modifying the counter */
	pthread_mutex_unlock(&mutex);
        printf("Mutex is unlocked from thread%ld\n",ThreadNum);

    printf("Thread%ld finished operation.The counter value = %d\n", ThreadNum, counter);
    pthread_exit(NULL);
}
int main(void){
	printf("Welcome to Thread synchronization using mutex(Static Intialization)\n");
	pthread_t threads[NUM_THREADS];
	int8_t s8Status;
	
	/*creating threads */
	 for(long t = 0; t < NUM_THREADS; t++){
        	s8Status = pthread_create(&threads[t], NULL,ThreadFunc, (void *)t);
        		if (s8Status) {
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




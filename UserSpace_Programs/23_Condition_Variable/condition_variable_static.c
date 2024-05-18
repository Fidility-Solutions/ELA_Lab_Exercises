/*
 * File       : thread_sync.c
 *
 * Description: This program demonstrates thread synchronization using condition variables and mutexes.
 *              It creates multiple threads that access a shared resource while ensuring mutual exclusion
 *              and synchronization using mutex locks and condition variables.
 *
 * Usage      : ./thread_sync nsec1 nsec2 ...
 *              where nsec1, nsec2, ... are the sleep times (in seconds) for each thread before accessing 
 *              the shared resource.
 *
 * Author     : Fidility Solutions
 *
 * Date       : 12/03/2024
 *
 * Reference  : The "Linux Programming Interface" book.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

/* static memory allocation to mutex(ThreadMutex) and cond var(ThreadDied) */
static pthread_cond_t ThreadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ThreadMutex = PTHREAD_MUTEX_INITIALIZER;

/* Error function to notify error and exit */
/*
 * Function   : errExit
 *
 * Description: Error handling function used to display an error message and terminate the program if an error occurs.
 *
 * Parameters : s8Msg - A string containing the error message to be displayed.
 *
 * Return     : This function does not return a value, as it exits the program upon encountering an error.
 */
void errExit(char *s8Msg){
	perror(s8Msg);
	exit(EXIT_FAILURE);
}
/* global static variables */
static int TotThreads = 0;
static int NumLive = 0;
static int NumUnJoind = 0;

/* To indentify the state of thread */
enum Tstate{
	TS_ALIVE,
	TS_TERMINATED,
	TS_JOINED
};
/* structure to store the thread id, state, sleeptime */
static struct ThreadInfo{
	pthread_t Tid;
	enum Tstate state;
	int SleepTime;
} *ThreadArray;

/*
 * Function   : threadfunc
 *
 * Description: Thread function that simulates the behavior of each thread. Each thread sleeps for a specified
 *              duration before accessing a shared resource. It then updates the shared resource and notifies
 *              the main thread using a condition variable after completing its task.
 *
 * Parameters : args - A pointer to the argument passed to the thread (thread ID).
 *
 * Return     : Returns NULL upon completion of thread execution.
 */
static void* threadfunc(void *args){
	int ThreadID = *((int *)args);
	int s;
	sleep(ThreadArray[ThreadID].SleepTime);
	/* lock the mutex */
	s= pthread_mutex_lock(&ThreadMutex);
	if(s != 0)
		errExit("pthread_mutex_lock error");
	/* critical section(shared resource) */
	printf("Thread %d acquired lock \n",ThreadID);
	printf("Thread%d accessed shared resource\n",ThreadID);
	/* Inc variable */
	NumUnJoind++;
	/* storing the state of the thread */
	ThreadArray[ThreadID].state=TS_TERMINATED;
	/* unlock the mutex */
	s=pthread_mutex_unlock(&ThreadMutex);
	if(s!=0)
		fprintf(stderr,"pthread mutex unlock error %d",s);
	printf("thread %d released lock \n",ThreadID);
	/* send the signal to waiting thread, then waiting thread resume the operation*/
	s= pthread_cond_signal(&ThreadDied);
	if(s!=0)
		errExit("pthread condition signal error");
	return NULL;
}
/*
 * Function   : main
 *
 * Description: Entry point of the program. Creates multiple threads to execute the threadfunc function.
 *              It also manages the synchronization of threads using mutex locks and condition variables
 *              to ensure mutual exclusion and proper termination of threads.
 *
 * Parameters : argc - The number of command-line arguments.
 *              argv - An array of strings containing the command-line arguments.
 *
 * Return     : Returns 0 on successful execution.
 */
int main(int argc,char *argv[]){
	printf("Welcome to Thread synchronization using condition varibale and mutex\n");
	
	/*variable declaration */
	int s, index;
	int ThreadArgs[argc-1];
	/* check the no. of arguments passed from user and any help */
	if(argc<2 || strcmp(argv[1],"--help") == 0){
		printf("Syntax		: filename thread1sleeptime thread2sleeptime ...\n\n");
		printf("filename	: name of the file to be executed\n");
		printf("thread1sleeptime: sleeptime of thread1\n");
		printf("thread2sleeptime: sleeptime of thread2\n");
		printf("...		: sleeptime to the number of threads\n");
		printf("Note		: The number of arguments gives the number of threads\n");
		fprintf(stderr,"Ex:%s 2 3\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	/* memory allocation to structure array to store the status of thread */
	ThreadArray = calloc(argc - 1, sizeof(struct ThreadInfo));
	/*create Threads */
	for(index=0;index<argc-1;index++){
		ThreadArgs[index]=index;
		s= pthread_create(&ThreadArray[index].Tid,NULL, threadfunc,&ThreadArgs[index]);
		if(s!=0)
			errExit("pthread_create error");
		/* storing sleep time this will help when the thread want to sleep */
		ThreadArray[index].SleepTime=atoi(argv[index+1]);
		ThreadArray[index].state=TS_ALIVE;
	}
	TotThreads = argc-1;
	NumLive = TotThreads;
	/*wait for all threads to terminate */
	while(NumLive>0){
		/* lock the mutex from main thread */
		s=pthread_mutex_lock(&ThreadMutex);
		if(s!=0)
			errExit("ptherad_mutex_lock error");
		printf("The main thread locked the mutex\n");
		/* this will check the no of thread joined */
		while(NumUnJoind ==0){
			printf("The main thread mutex lock is released due to the wait condition\n");
			/* waiting for signal to resume the opeation: when the wait executed the mutex lock 
			 * release from main thread so other thread will aquire the lock and when the signal 
			 * send form the thread again main thread resume the operation*/
			s= pthread_cond_wait(&ThreadDied,&ThreadMutex);
			printf("The main thread acquires the lock after receiving the thread signal\n");
			if(s!=0)
				errExit("pthread_cond_wait error");
				}
		/* waiting for thread to terminated */
		for(index=0;index<TotThreads;index++){
			if(ThreadArray[index].state == TS_TERMINATED){
				s=pthread_join(ThreadArray[index].Tid,NULL);
				if(s!=0)
					errExit("pthread join error");
				ThreadArray[index].state= TS_JOINED;
				NumLive--;
				NumUnJoind--;
				printf("Reaped Thread %d(NumLive =%d)\n",index,NumLive);
			}
		}
		/* unlock the mutex after threads joined */
		s=pthread_mutex_unlock(&ThreadMutex);
		if(s!=0)
			errExit("pthread_mutex_unlock error");
		printf("The main thread released the mutex\n");
	}
		printf("All threads completed\n");
		exit(EXIT_SUCCESS);
}



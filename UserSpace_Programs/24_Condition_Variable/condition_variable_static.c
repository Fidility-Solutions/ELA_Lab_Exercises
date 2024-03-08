#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>

static pthread_cond_t ThreadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t ThreadMutex = PTHREAD_MUTEX_INITIALIZER;
void errExit(char *s8Msg){
	perror(s8Msg);
	exit(EXIT_FAILURE);
}
static int TotThreads = 0;
static int NumLive = 0;
static int NumUnJoind = 0;
enum Tstate{
	TS_ALIVE,
	TS_TERMINATED,
	TS_JOINED
};
static struct ThreadInfo{
	pthread_t Tid;
	enum Tstate state;
	int SleepTime;
} *ThreadArray;
static void* threadfunc(void *args){
	int ThreadID = *((int *)args);
	int s;
	sleep(ThreadArray[ThreadID].SleepTime);
	//printf("Thread %d g\n",ThreadID);
	s= pthread_mutex_lock(&ThreadMutex);
	if(s!=0)
		errExit("pthread_mutex_lock error");
	printf("Thread %d acuquired lock \n",ThreadID);
	printf("Thread%d accessed shared resource\n",ThreadID);
	NumUnJoind++;
	ThreadArray[ThreadID].state=TS_TERMINATED;
	s=pthread_mutex_unlock(&ThreadMutex);
	if(s!=0)
		fprintf(stderr,"pthread mutex unlock error %d",s);
	printf("thread %d released lock \n",ThreadID);
	s= pthread_cond_signal(&ThreadDied);
	if(s!=0)
		errExit("pthread condition signal error");
	return NULL;
}
int main(int argc,char *argv[]){
	int s, index;
	int ThreadArgs[argc-1];
//	pthread_t Threads[argc-1];
	if(argc<2 || strcmp(argv[1],"--help") == 0)
		fprintf(stderr,"%s nsec...\n",argv[0]);
//	ThreadArray= calloc(argc-1,sizeof(ThreadArray));
	ThreadArray = calloc(argc - 1, sizeof(struct ThreadInfo));
	/*create Threads */
	for(index=0;index<argc-1;index++){
		ThreadArgs[index]=index;
		s= pthread_create(&ThreadArray[index].Tid,NULL, threadfunc,&ThreadArgs[index]);
		if(s!=0)
			errExit("pthread_create error");
		ThreadArray[index].SleepTime=atoi(argv[index+1]);
		ThreadArray[index].state=TS_ALIVE;
	}
	TotThreads = argc-1;
	NumLive = TotThreads;
	/*wait for all threads to terminate */
	while(NumLive>0){
		s=pthread_mutex_lock(&ThreadMutex);
//		printf("The main thread locked the mutex\n");
		if(s!=0)
			errExit("ptherad_mutex_lock error");
		printf("The main thread locked the mutex\n");
		while(NumUnJoind ==0){
		//	sleep(3);
			printf("released\n");
			s= pthread_cond_wait(&ThreadDied,&ThreadMutex);
			if(s!=0)
				errExit("pthread_cond_wait error");
			printf("The main thread called wait condition so it released mutex lock\n");
		}
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
		s=pthread_mutex_unlock(&ThreadMutex);
		if(s!=0)
			errExit("pthread_mutex_unlock error");
		printf("The main thread released the mutex\n");
	}
		printf("All threads completed\n");
		exit(EXIT_SUCCESS);
}


	

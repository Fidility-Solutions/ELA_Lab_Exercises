#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define SEM_NAME "/named_semaphore"

/* Error notify and exit function */
void errExit(const char *ErrMsg) {
    perror(ErrMsg);
    exit(EXIT_FAILURE);
}
/*Global variable Declaration */
int *ps8count;
/*pointer to semaphore */
sem_t *sem;
void ParentProcess(void)
{
	/* value to check current semaphore */
	int u8Val;
	printf("\nThis is parent process\n");
	/* The sem_wait() sys call decrement (by 1) the value of semaphore if it is greater than 0 */
	printf("Waiting for semaphore in parent process...\n");
	if(sem_wait(sem)== -1)
		errExit("sem_wait failed");
	/* To get the current value of semaphore we will use sem_getvalue() sys call */
	if(sem_getvalue(sem,&u8Val) == -1)
                errExit("sem_getu8Val error");
	printf("Semaphore acquired! The sem value after wait in parent process is:%d\n",u8Val);

	/* perform action from parent */
	if(*ps8count>1)
		printf("The counter value is :%d\n",*ps8count);
	else{
	for(int i=0;i<10;i++)
		(*ps8count)++;
	printf("The counter value  after operation in parent is :%d\n",*ps8count);
	}
	/* Increment the semaphore value using sem_post () sys call */
	if(sem_post(sem) == -1)
		errExit("sem_post error");
	/* Get the current value after post */
	if(sem_getvalue(sem,&u8Val) == -1)
		errExit("sem_getu8Val error");
	printf("Semaphore released! The sem value after post from parent process is:%d\n",u8Val);
	/* wait fro child to finish work*/
	wait(NULL);
}
void ChildProcess(void){
	/* Value used for to check value of semaphore */
	int u8Val;
	printf("\nThis is child Process\n");

	/* Get the current value of semaphore before proceed*/
	if(sem_getvalue(sem,&u8Val) == -1)
                errExit("sem_getu8Val error");
	printf("The sem value before wait in child process is:%d\n",u8Val);
	/* Decrement the value of semaphore if it is greater than 0 */
	printf("Waiting for semaphore in child process...\n");
	if(sem_wait(sem)== -1)
		errExit("sem wait error from child");

	/* perform some action */
	if(sem_getvalue(sem,&u8Val) == -1)
                errExit("sem_getu8Val error");
	printf("Semaphore acquired! The semaphore value after wait in child process is:%d\n",u8Val);

	if(*ps8count <1)
		printf("The count value is: %d\n",*ps8count);
	else{
		for(int i=0;i<10;i++)
			(*ps8count)++;
		printf("The count value in child after operation on count is: %d\n",*ps8count);
	}
	/* Increment the semaphore value */
	if(sem_post(sem) == -1)
		errExit("sem post error from child");
	
	/* After post check the value of semaphore */
	if(sem_getvalue(sem,&u8Val) == -1)
                errExit("sem_getu8Val error");
	printf("Semaphore released! The sem value after post in child process is:%d\n",u8Val);

	exit(1);
}
int main(void){
	printf("Welcome to POSIX named semaphore program\n");
	/* mmap() is a POSIX function used to map a region of memory into the address space of a process. By specifying the MAP_SHARED flag, we indicate that the mapped memory region should be shared among multiple processes. */
	ps8count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	/* Specify the initial count value */
	*ps8count =1;
	/* Create/Open the sempahore */
	sem=sem_open(SEM_NAME,O_CREAT|O_EXCL, S_IRUSR | S_IWUSR,1);
	if(sem==SEM_FAILED)
		errExit("semaphore creation failed");
	/*fork child */
	pid_t pid=fork();
	if(pid == -1)
		errExit("fork faild");
	else if(pid == 0)
		ChildProcess();
	else
		ParentProcess();

	/* Closing the semaphore */
    if (sem_close(sem) == -1)
        errExit("sem_close error");
    printf("\nThe semaphore is closed \n");

    /* Unlinking the semaphore */
    if (sem_unlink(SEM_NAME) == -1)
        errExit("sem_unlink error");
    printf("The semaphore is deleted \n");
    printf("Exiting...\n");

    return 0;
}

	
	





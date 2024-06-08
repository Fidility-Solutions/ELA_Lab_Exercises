/******************************************************************************
 * File:        named_semaphore_using_process.c
 *
 * Description: This program demonstrates the usage of POSIX named semaphores for inter-process synchronization. 
 *              It creates a named semaphore, forks a child process, and then both parent and child processes 
 *              manipulate a shared counter while ensuring mutual exclusion using the semaphore.
 *
 * Usage:       ./named_semaphore_using_process
 * 
 * Author:      Fidility Solutions
 *  
 * Date:        5/03/2024
 *
 * Reference:   "The Linux Programming Interface" book
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define SEM_NAME "/named_semaphore"

/* Error notify and exit function */
void errExit(const char *ErrMsg) {
    perror(ErrMsg);
    exit(EXIT_FAILURE);
}
/*Global variable Declaration */
uint8_t *ps8count;
/*pointer to semaphore */
sem_t *sem;




/* Function	: ParentProcess
 * ------------------------
 * Description	: Parent process function that waits for the semaphore, performs actions, and releases the semaphore.
 * 
 * Parameters	: None
 * 
 * Returns	: None
 */
void ParentProcess(void)
{
	/* value to check current semaphore */
	int RetriveValue;
	printf("\nThis is parent process\n");
	/* The sem_wait() sys call decrement (by 1) the value of semaphore if it is greater than 0 */
	printf("Waiting for semaphore in parent process...\n");
	if(sem_wait(sem)== -1)
		errExit("sem_wait failed");
	/* To get the current value of semaphore we will use sem_getvalue() sys call */
	if(sem_getvalue(sem,&RetriveValue) == -1)
                errExit("sem_getRetriveValue error");
	printf("Parent acquired semaphore! The sem value after wait is:%d\n",RetriveValue);

	/* perform action from parent */
	if(*ps8count>1)
		printf("The counter value is :%d\n",*ps8count);
	else{
	for(int i=0;i<10;i++)
		(*ps8count)++;
	printf("Counter value  after operation in parent is :%d\n",*ps8count);
	}
	/* Increment the semaphore value using sem_post () sys call */
	if(sem_post(sem) == -1)
		errExit("sem_post error");
	/* Get the current value after post */
	if(sem_getvalue(sem,&RetriveValue) == -1)
		errExit("sem_getRetriveValue error");
	printf("Parent released semaphore! The sem value after post is:%d\n",RetriveValue);
	/* wait fro child to finish work*/
	wait(NULL);
}
/* Function	: ChildProcess
 * -----------------------
 * Description	: Child process function that waits for the semaphore, performs actions, and releases the semaphore.
 *
 * Parameters	: None
 *
 * Returns	: None
 */
void ChildProcess(void){
	/* Value used for to check value of semaphore */
	int RetriveValue;
	printf("\nThis is child Process\n");

	/* Get the current value of semaphore before proceed*/
	if(sem_getvalue(sem,&RetriveValue) == -1)
                errExit("sem_getRetriveValue error");
	printf("Sem value before wait in child process is:%d\n",RetriveValue);
	/* Decrement the value of semaphore if it is greater than 0 */
	printf("Waiting for semaphore in child process...\n");
	if(sem_wait(sem)== -1)
		errExit("sem wait error from child");

	/* perform some action */
	if(sem_getvalue(sem,&RetriveValue) == -1)
                errExit("sem_getRetriveValue error");
	printf("Child process acquired semaphore! The semaphore value after wait is:%d\n",RetriveValue);

	if(*ps8count <1)
		printf("The count value is: %d\n",*ps8count);
	else{
		for(int i=0;i<10;i++)
			(*ps8count)++;
		printf("Count value in child after operation on count is: %d\n",*ps8count);
	}
	/* Increment the semaphore value */
	if(sem_post(sem) == -1)
		errExit("sem post error from child");
	
	/* After post check the value of semaphore */
	if(sem_getvalue(sem,&RetriveValue) == -1)
                errExit("sem_getRetriveValue error");
	printf("Child Process released semaphore! The sem value after post is:%d\n",RetriveValue);

	exit(EXIT_SUCCESS);
}
/* Function	: main
 * ---------------
 * Description	: The main function of the program. It initializes the shared memory region, creates a named semaphore, 
 *              forks a child process, and executes the parent or child function based on the process ID.
 * 
 * Parameters	: None
 * 
 * Returns	: 0 upon successful execution of the program
 */
int main(void){
	printf("Welcome to POSIX named semaphore program using processes\n");
	/* mmap() is a POSIX function used to map a region of memory into the address space of a process. By specifying the MAP_SHARED flag, we indicate that the mapped memory region should be shared among multiple processes. */
	ps8count = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	/* Specify the initial count value */
	*ps8count =1;
	/* Attempt to unlink the semaphore if it already exists */
    	if(sem_unlink(SEM_NAME) == -1 && errno != ENOENT) {
        	perror("sem_unlink");
        	exit(EXIT_FAILURE);
    	}
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

	
	





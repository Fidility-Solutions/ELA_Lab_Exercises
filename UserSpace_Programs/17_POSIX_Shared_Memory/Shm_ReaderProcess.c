

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SHARED_MEM_SIZE 1024
#define SHARED_MEM_NAME "/my_shared_memory"


int  main() {
	printf("Welcome to POSIX Shared Memory Reader Process Program \n");

    	/* Create shared memory object */
	printf("\nShared Memory is created in shared memory reader process \n");
    	int shm_fd = shm_open(SHARED_MEM_NAME, O_CREAT | O_RDWR, 0666);
    	if (shm_fd == -1) {
        	perror("shm_open error");
        	exit(EXIT_FAILURE);
    	}

    	/* Set the size of the shared memory object */
	printf("Setting the Size to Shared Memory Object Using ftruncate () system call...\n");
    	if (ftruncate(shm_fd, SHARED_MEM_SIZE) == -1) {
        	perror("ftruncate error");
        	exit(EXIT_FAILURE);
    	}

    	/* Map the shared memory object into the address space of the Reader process */
	printf("Mapping the shared memory object into the address space ..\n");
    	char *parent_shared_memory = mmap(NULL, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    	if (parent_shared_memory == MAP_FAILED) {
        	perror("mmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Create a child process */
	printf("\nThe child process created from reader process it will replaced with writter process\n");
    	pid_t pid = fork();

    	if (pid < 0) {
        	perror("fork error");
        	exit(EXIT_FAILURE);
    	}
	/*child process */
    	else if (pid == 0) {
		printf("The Child process id(PID):%d\n",getpid());
		printf("The child process from shared memory reader process is replaced with shared memory writter process using execl system call\n");
        	execl("./Shm_WritterProcess", "Shm_WritterProcess", (char *)NULL);
        	perror("exec error");
        	exit(EXIT_FAILURE);
    	} 
	/* Parent Process */
	else {
		wait(NULL);
		printf("The Writter Process is Terminated!\n");
		printf("The Reader process id(PID):%d\n",getpid());
        	printf("\nReader process reading from shared memory...\n");
        	printf("Reader process read: %s \n", parent_shared_memory);
    	}
	printf("\nUnmapping Shared Memory in Shared Memory Reader Process\n");
    	/* Unmap shared memory in the parent process */

    	if (munmap(parent_shared_memory, SHARED_MEM_SIZE) == -1) {
        	perror("munmap error");
        	exit(EXIT_FAILURE);
    	}

    	/* Close and unlink shared memory object */
	printf("Closing Shared Memory Object \n");
    	if (close(shm_fd) == -1) {
        	perror("close error");
        	exit(EXIT_FAILURE);
    	}
	printf("Deleting Shared Memory Object...\n");
    	if (shm_unlink(SHARED_MEM_NAME) == -1) {
        	perror("shm_unlink");
        	exit(EXIT_FAILURE);
    	}
	printf("Exited Shared Memory Reader Process\n");
	printf("\nThank you\n");

    	return 0;
}


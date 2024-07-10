
/*
 * File name	: Event_Fd.c
 *
 * Description	: Demonstrates the usage of event file descriptors for inter-process communication 
 * 		  in both thread-based and process-based modes.
 * Usage	: ./Event_Fd <mode>
 *      		Mode: "thread" or "process <event numbers>"
 *      		Example1: ./Event_Fd thread
 *      		Example2: ./Event_Fd process 1 2 4
 *
 * Date		: 21/03/2024
 *
 * Author	:Fidility Solutions
 *
 * Reference	: Linux manual page
*/

#define _GNU_SOURCE /* Required for eventfd.h on some systems */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <pthread.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <sys/types.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define NUM_THREADS 3
/*
 * Function: 	ThreadFn
 *
 * Description: Represents the behavior of each thread. It reads events from the event file descriptor.
 *
 * Parameters: 	void pointer to the event file descriptor
 *
 * Return: 	void pointer
 *
 */
void *ThreadFn(void *arg){
	 pthread_t tid = gettid(); 
	/* Extract the eventfd file descriptor from the argument */
    	int EvntFd = *(int *)arg;
	/* Variable to hold the value read from eventfd */
    	uint64_t value;
	/* Read from eventfd. This call will block until an event is available */
    	if(eventfd_read(EvntFd, &value) == -1)
        handle_error("eventfd_read");
	/* Print the thread ID and the value received from eventfd */
    	printf("Thread %lu: Event received, counter value: %ld\n",tid, value);

    	pthread_exit(NULL);
}
/*
 * Function: 	main
 *
 * Description: The main function of the program. Parses the mode of operation from the command-line arguments 
 * 		and demonstrates event file descriptor usage accordingly.
 *
 * Parameters: 	argc - number of command-line arguments, 
 * 		argv - array of command-line arguments
 *
 * Return: 	0 on successful.
 *
 */
int main(int argc, char *argv[]){
	printf("Welcome to the Event File Descriptor synchronization mechanism for inter-process communication\n");

	/* Eventfd file descriptor */
    	int EvntFd;
	/* Variable to hold value to be written to eventfd */
    	uint64_t HoldVal;
	uint64_t value = 0;
	/* Return value of read/write operations */
    	ssize_t ReturnVal;
	/* Variable to hold the mode of operation (thread/process) */
    	char mode[10];
	/* Check if the correct number of command-line arguments are provided */
    	if(argc < 2){
		/* Print usage instructions if not enough arguments are provided */
		fprintf(stderr, "Usage: %s <mode>\n"
                "Mode:\"thread\" or \"process <event numbers>\"\n"
                "Example1: %s thread\n"
                "Example2: %s process 1 2 4\n", argv[0], argv[0], argv[0]);

        	exit(EXIT_FAILURE);
    	}
	/* Parse the mode of operation from the command-line arguments */
    	if(sscanf(argv[1], "%s", mode) != 1) {
        	fprintf(stderr, "Invalid mode\n");
        	exit(EXIT_FAILURE);
    	}
	/* If the mode of operation is thread-based */
    	if(strcmp(mode, "thread") == 0){

		/* Thread-based eventfd */
		/* Array to hold thread IDs */
      		pthread_t threads[NUM_THREADS];
		/* Create an eventfd with semaphore mode */
        	EvntFd = eventfd(0,0);
        	if(EvntFd == -1)
            		handle_error("eventfd");
		/* Create multiple threads */
        	for(int i = 0; i < NUM_THREADS; ++i){
            		if(pthread_create(&threads[i], NULL, ThreadFn, &EvntFd) != 0)
                		handle_error("pthread_create");
        	}
		/* Write values to the eventfd to signal events to threads */
        	for(int i = 0; i < NUM_THREADS; ++i){
			sleep(2);
			/* Value to be written to eventfd */
            		value++;
			/* Handle error if write to eventfd fails */
            		if(eventfd_write(EvntFd, value) == -1)
                		handle_error("eventfd_write");
            		printf("Main: Event sent, counter value: %"PRIu64"\n", value);
        	}
		/* Wait for all threads to complete */
        	for(int i = 0; i < NUM_THREADS; ++i){
            	if(pthread_join(threads[i], NULL) != 0)
                	handle_error("pthread_join");
        	}
		/* Close the eventfd */
        	if(close(EvntFd) == -1)
            		handle_error("close");
    	} 
	/* If the mode of operation is process-based */
    	else if(strcmp(mode, "process") == 0){

		if(argc < 3){
                /* Print usage instructions if not enough arguments are provided */
                fprintf(stderr, "Usage: %s <mode> <event number>\n"
                "Example2: %s process 1 2 4\n", argv[0], argv[0]);
                exit(EXIT_FAILURE);
        }

		/* Process-based eventfd */
		/* Create an eventfd with semaphore mode and non-blocking flag */
        	/* EvntFd = eventfd(0, EFD_SEMAPHORE | EFD_NONBLOCK); */
		EvntFd = eventfd(0,0);
        	if(EvntFd == -1)
            		handle_error("eventfd");

        switch(fork()){
            	case 0:
		    	/* Child process writes values to the eventfd */
                	for(size_t j = 2; j < argc; j++){
                    		printf("Child writing %s to efd\n", argv[j]);
                    		HoldVal = strtoull(argv[j], NULL, 0);
                    		ReturnVal = write(EvntFd, &HoldVal, sizeof(uint64_t));
                    	
				if(ReturnVal != sizeof(uint64_t))
                        		handle_error("write");
                	}
                	printf("Child completed write loop\n");
                	exit(EXIT_SUCCESS);

            	default:
			/* Parent process reads values from the eventfd */
			sleep(2);
                    	printf("Parent about to read\n");
                    	ReturnVal = read(EvntFd, &HoldVal, sizeof(uint64_t));
                    	if (ReturnVal != sizeof(uint64_t)){
					printf("Eventfd is empty. No events to read.\n");
                        		exit(EXIT_SUCCESS);
                        	}
			printf("Parent read %"PRIu64" (%#"PRIx64") from efd\n", HoldVal,HoldVal);
                	
            	case -1:
			/* Handle error if fork fails */
                	handle_error("fork");
        	}
    	} 
	/* If an invalid mode is provided */
    	else{
		/* Print error message for invalid mode */
        	fprintf(stderr, "Invalid mode\n");
        	exit(EXIT_FAILURE);
    	}

    	return 0;
}




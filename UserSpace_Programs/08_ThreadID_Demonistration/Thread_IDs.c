#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

/*
 * Function:    threadfunction()
 *
 * Description: This function represents the code executed by each thread. It receives
 *              a thread ID and a message as arguments. Based on the message received,
 *              it performs different operations.
 *
 * Arguments: 	arg: A pointer to void representing the arguments passed to the thread function.
 *           	The first sizeof(int) bytes contain the thread ID, and the remaining bytes
 *           	contain the message.
 * 
 * Returns:     NULL: Since the function is of type void *, it returns NULL to indicate successful completion 
 *            of the thread function.
 *
 */
void *thread_function(void *arg) {
    // Extract arguments passed to the thread
    int thread_id = *((int *)arg);
    char *message = (char *)arg + sizeof(int); // Pointer arithmetic to get message

    // Print the Thread ID (TID) and the message received by the thread
    printf("Thread ID(TID): %ld, Message: %s\n", pthread_self(), message);

    // Perform some operation based on the thread ID or message received
     // Perform different operations based on the message received
    if (strcmp(message, "Hello") == 0) {
        printf("Thread %d says Hello!\n", thread_id);
    } else if (strcmp(message, "Goodbye") == 0) {
        printf("Thread %d says Goodbye!\n", thread_id);
    } else {
        printf("Thread %d says Unknown message: %s\n", thread_id, message);
    }
    // Return a value indicating successful execution
    return NULL;
}
/*
 * Function:    main()
 *
  * Description:This function represents the entry point of the program. It creates
 *              multiple threads, each with its own unique arguments, and waits for
 *              all threads to finish execution.
 *
 * Arguments:   None
 *  
 * Returns:     0: Indicates successful execution of the program.
 *      	1: Indicates an error occurred during thread creation or joining.
 *
 */


int main() {
    	printf("Entered into main program\n");
	/* Array to hold thread IDs */
    	pthread_t threads[3]; // Array to hold thread IDs
	/* Arguments for each thread */
	struct thread_args_struct {
        	int thread_id;
        	char *message;
    	} thread_args[3] = {
        	{1, "Hello from Thread 1"},
        	{2, "Greetings from Thread 2"},
        	{3, "Hi there from Thread 3"}
    		}; 

    	/* Create multiple threads */
    	for (int i = 0; i < 3; i++) {
        	if (pthread_create(&threads[i], NULL, thread_function, (void *)&thread_args[i]) != 0) {
            	fprintf(stderr, "Error creating thread %d.\n", i + 1);
            	return 1;
        	}
    	}

    	/* Wait for all threads to finish */
    	for (int i = 0; i < 3; i++) {
        	if (pthread_join(threads[i], NULL) != 0) {
            	fprintf(stderr, "Error joining thread %d.\n", i + 1);
            	return 1;
        	}
    	}

    	printf("Main thread exiting.\n");
    	return 0;
}


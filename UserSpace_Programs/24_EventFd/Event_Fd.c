#define _GNU_SOURCE // Required for eventfd.h on some systems

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/eventfd.h>
#include <pthread.h>
#include <errno.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define NUM_THREADS 3

// Function to be executed by each thread
void *thread_function(void *arg) {
    int efd = *(int *)arg;

    // Wait for event
    uint64_t value;
    if (eventfd_read(efd, &value) == -1)
        handle_error("eventfd_read");

    printf("Thread %ld: Event received, counter value: %lu\n", pthread_self(), value);

    pthread_exit(NULL);
}

int main() {
    int efd;
    uint64_t value = 0;

    // Create eventfd object
    efd = eventfd(0, EFD_SEMAPHORE);
    if (efd == -1)
        handle_error("eventfd");

    // Create threads
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_create(&threads[i], NULL, thread_function, &efd) != 0)
            handle_error("pthread_create");
    }

    // Perform operations on eventfd
    for (int i = 0; i < NUM_THREADS; ++i) {
        // Increment event counter by 1
        value++;
        if (eventfd_write(efd, value) == -1)
            handle_error("eventfd_write");
        printf("Main: Event sent, counter value: %lu\n", value);
    }

    // Wait for threads to finish
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], NULL) != 0)
            handle_error("pthread_join");
    }

    // Close eventfd
    if (close(efd) == -1)
        handle_error("close");

    return 0;
}


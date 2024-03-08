#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Structure to hold data shared between threads
struct SharedData {
    int value;
    pthread_mutex_t* mutex;     // Pointer to dynamically allocated mutex
    pthread_cond_t* cond;       // Pointer to dynamically allocated condition variable
};

// Function executed by threads
void* thread_func(void* arg) {
    struct SharedData* shared_data = (struct SharedData*)arg;

    // Lock the mutex
    pthread_mutex_lock(shared_data->mutex);

    // Wait for the condition variable to be signaled
    pthread_cond_wait(shared_data->cond, shared_data->mutex);

    // Condition variable has been signaled, perform some action
    printf("Thread %ld: Value after condition signal: %d\n", pthread_self(), shared_data->value);

    // Unlock the mutex
    pthread_mutex_unlock(shared_data->mutex);

    return NULL;
}

int main() {
    // Initialize shared data
    struct SharedData shared_data;
    shared_data.value = 0;

    // Dynamically allocate memory for mutex
    shared_data.mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (shared_data.mutex == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Dynamically allocate memory for condition variable
    shared_data.cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
    if (shared_data.cond == NULL) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Initialize mutex
    if (pthread_mutex_init(shared_data.mutex, NULL) != 0) {
        perror("pthread_mutex_init failed");
        exit(EXIT_FAILURE);
    }

    // Initialize condition variable
    if (pthread_cond_init(shared_data.cond, NULL) != 0) {
        perror("pthread_cond_init failed");
        exit(EXIT_FAILURE);
    }

    // Create threads
    pthread_t thread1, thread2;
    if (pthread_create(&thread1, NULL, thread_func, &shared_data) != 0 ||
        pthread_create(&thread2, NULL, thread_func, &shared_data) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    // Lock the mutex
    pthread_mutex_lock(shared_data.mutex);

    // Signal the condition variable to wake up waiting threads
    shared_data.value = 42;
    pthread_cond_signal(shared_data.cond);

    // Unlock the mutex
    pthread_mutex_unlock(shared_data.mutex);

    // Wait for threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    // Destroy mutex and condition variable
    pthread_mutex_destroy(shared_data.mutex);
    pthread_cond_destroy(shared_data.cond);

    // Free memory
    free(shared_data.mutex);
    free(shared_data.cond);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define CHUNK_SIZE (500 * 1024 * 1024) // 100MB

int main() {
    printf("Initial memory usage:\n");
    system("free -m");

    printf("Allocating memory...\n");
    while (1) {
        void *chunk = malloc(CHUNK_SIZE);
        if (!chunk) {
            perror("malloc");
            break;
        }
        // Touch the allocated memory to ensure it's actually allocated
        memset(chunk, 0, CHUNK_SIZE);
        sleep(1); // Slow down the allocation a bit
    }

    printf("Final memory usage:\n");
    system("free -m");

    return 0;
}


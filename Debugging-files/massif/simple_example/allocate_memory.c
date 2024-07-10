#include <stdio.h>
#include <stdlib.h>

void allocateMemory(int size) {
    /* Function to allocate memory */
    int *ptr = (int *)malloc(size * sizeof(int));
    if (ptr == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    printf("Memory allocated successfully in allocateMemory function.\n");
}

void process(int *data, int size) {
    /* Function to process data */
    printf("Processing data in process function...\n");
    for (int i = 0; i < size; i++) {
        data[i] *= 2;
    }
}

int main() {
    int n = 10;
    int *array = (int *)malloc(n * sizeof(int));
    if (array == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    /* Simulating memory allocation and deallocation in different functions */
    allocateMemory(5);
    process(array, n);

   free(array);
    printf("Memory freed in main function.\n");

    return 0;
}

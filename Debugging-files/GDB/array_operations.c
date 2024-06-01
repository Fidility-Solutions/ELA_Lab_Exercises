#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE 10
#define BUFFER_SIZE 256
#define FILENAME "data.txt"

void write_array_to_file(const char* filename, int* array, size_t size) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < size; ++i) {
        fprintf(file, "%d\n", array[i]);
    }

    fclose(file);
}

void read_file_and_print(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        printf("Read line: %s", buffer);
    }

    fclose(file);
}

int find_max(int* array, size_t size) {
    int max = array[0];
    for (size_t i = 1; i < size; ++i) {
        if (array[i] > max) {
            max = array[i];
        }
    }
    return max;
}

void sort_array(int* array, size_t size) {
    for (size_t i = 0; i <=size; ++i) { // Introduce bug here: change size to size - 1
        for (size_t j = 0; j < size - 1 - i; ++j) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

int main() {
    int array[ARRAY_SIZE] = {5, 3, 8, 6, 2, 7, 4, 1, 9, 0};

    write_array_to_file(FILENAME, array, ARRAY_SIZE);
    read_file_and_print(FILENAME);

    int max_value = find_max(array, ARRAY_SIZE);
    printf("Max value: %d\n", max_value);

    sort_array(array, ARRAY_SIZE);

    printf("Sorted array: ");
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}


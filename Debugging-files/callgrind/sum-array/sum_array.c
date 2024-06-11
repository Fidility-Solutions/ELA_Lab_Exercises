#include <stdio.h>

#define SIZE 1000

int sum_array(int arr[], int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += arr[i];
    }
    return sum;
}

int main() {
    int arr[SIZE];
    for (int i = 0; i < SIZE; i++) {
        arr[i] = i;
    }

    int result = sum_array(arr, SIZE);
    printf("Sum: %d\n", result);

    return 0;
}


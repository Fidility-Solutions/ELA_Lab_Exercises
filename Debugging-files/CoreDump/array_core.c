#include <stdio.h>
#include <stdlib.h>

#define ROWS 1000
#define COLS 1000

void processMatrix(int **matrix) {
    	for (int i = 0; i < ROWS; i++) {
        	for (int j = 0; j < COLS; j++) {
			/* Filling the matrix with product of row and column indices*/
            		matrix[i][j] = i * j;
        	}
    	}
}

void accessMatrixElement(int **matrix) {
   	 printf("Element at [1001][1001]: %d\n", matrix[1001][1001]); // Intentional segmentation fault
}

int main() {
    int **matrix = NULL;

    /* Allocate memory for a 2D array dynamically */
    matrix = (int **)malloc(ROWS * sizeof(int *));
    if (matrix == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < ROWS; i++) {
        matrix[i] = (int *)malloc(COLS * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Memory allocation failed!\n");
            return 1;
        }
    }

    /* Fill the matrix with product of row and column indices */
    processMatrix(matrix);

    /* Access an element beyond the allocated memory */
    accessMatrixElement(matrix);

    /* Free allocated memory */
    for (int i = 0; i < ROWS; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int main() {
    FILE *file;
    char buffer[BUFFER_SIZE];

    /* Open file */
    file = fopen("large_file.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    /* Read file and process data */
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        /* Count the number of characters in each line */
        int char_count = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] != '\n' && buffer[i] != '\r') {
                char_count++;
            }
        }
        printf("Line length: %d\n", char_count);
    }

    /* Close file */
    fclose(file);

    return 0;
}


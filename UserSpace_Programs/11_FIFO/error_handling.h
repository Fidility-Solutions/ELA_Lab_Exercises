#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>
#include <stdlib.h>

// Function to handle errors with perror and exit
void handle_error(const char *message);

// Function to display usage error and exit
void usageErr(const char *programName, const char *message);

// Function to display error message and exit
void ErrExit(const char *format, ...);

#endif /* ERROR_HANDLING_H */


#include "error_handling.h"
#include <stdarg.h>

void errExit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void usageErr(const char *programName, const char *message) {
    fprintf(stderr, "Usage: %s %s\n", programName, message);
    exit(EXIT_FAILURE);
}

void ErrExit(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}


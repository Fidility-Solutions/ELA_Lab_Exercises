#include "fifo.h"

void usageErr(const char *programName, const char *message) {
    fprintf(stderr, "Usage: %s %s\n", programName, message);
    exit(EXIT_FAILURE);
}

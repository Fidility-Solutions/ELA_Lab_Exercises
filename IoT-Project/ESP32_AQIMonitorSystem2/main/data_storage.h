#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <stdint.h>
#include <stddef.h>

#define COUNTER_INITIAL_VALUE 		16
#define CLOUD_COUNTER_INITIAL_VALUE 	16
#define MAX_ADDRESS 			(16 * 256 * 256)


void dataStorageTask(void *pvParameters);


#endif /* DATA_STORAGE_H */

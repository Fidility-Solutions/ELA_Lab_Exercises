#ifndef BME680_SENSOR_H_
#define BME680_SENSOR_H_

//#include "main.h"

#define PORT 		0
#define ADDR_BME680 	0x76

#define CONFIG_EXAMPLE_I2C_MASTER_SDA   21   /* Change to your SDA pin */
#define CONFIG_EXAMPLE_I2C_MASTER_SCL   22   /* Change to your SCL pin */

#define BME680_TASK_STACK_SIZE    	2048
#define BME680_TASK_PRIORITY       	3

/**
 * Starts the BME680 task
 */

void BME680_task_start(void);

#endif /* BME680_SENSOR_H_ */

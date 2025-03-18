#ifndef MAIN_BME680_SENSOR_H_
#define MAIN_BME680_SENSOR_H_

/**
 * Starts the BME680 task
 */

#define BME680_TASK_STACK_SIZE    2048
#define BME680_TASK_PRIORITY       3


void BME680_task_start(void);

#endif /* MAIN_BME680_SENSOR_H_ */

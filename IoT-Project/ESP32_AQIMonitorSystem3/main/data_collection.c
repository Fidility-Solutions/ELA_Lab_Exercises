/*******************************************************************************
 * File        : data_collection.c
 *
 * Description : This source file implements the FreeRTOS task responsible for
 *               collecting environmental sensor data using BME680, MQ135, and SDS011.
 *               It synchronizes sensor access using a semaphore, reads the data,
 *               processes it (including gas PPM calculations and AQI), and updates
 *               the global shared data structure for use by other tasks.
 *
 *               Key Functionalities:
 *               - BME680 temperature and humidity measurement
 *               - MQ135 raw ADC reading and PPM calculation
 *               - SDS011 PM2.5 and PM10 data reading
 *               - AQI computation
 *               - Location tagging and time synchronization
 *               - Task synchronization using FreeRTOS semaphores
 *
 * Author      : Fidility Solutions
 *
 * Reference   : BME680 datasheet, MQ135 gas sensor documentation,
 *               SDS011 laser dust sensor specs,
 *               FreeRTOS API documentation.
 *******************************************************************************/



/* Include Header files */
#include "data_collection.h"
#include "rtc.h"
#include "main.h"
#include "bme680.h"
#include "bme680_sensor.h"
#include <string.h>
#include "esp_random.h"
extern bme680_t sensor;  				/* Global sensor object */

extern bme680_values_float_t values;  			/* Global sensor values */

extern STR_SDS011_SENSOR_DATA str_sds011_sensor_data;   /* Global sensor values */

STR_SENSOR_DATA str_global_sensor_data;

extern uint32_t duration;


/*
 * Function    : dataCollectionTask()
 *
 * Description : FreeRTOS task responsible for collecting and updating environmental
 *               data periodically. It handles:
 *               - Forcing a BME680 sensor measurement and retrieving temperature/humidity.
 *               - Reading MQ135 gas sensor value and converting to PPM.
 *               - Reading SDS011 sensor values for PM2.5 and PM10.
 *               - Calculating and storing AQI.
 *               - Optionally synchronizing current time via NTP.
 *               - Storing data into the shared global structure.
 *               - All operations are synchronized using a FreeRTOS semaphore.
 *
 * Parameters  :
 *      - pvParameters : Pointer to optional task parameters (unused in this task).
 *
 * Returns     : This function runs as an infinite loop, hence does not return.
 */

void dataCollectionTask(void *pvParameters)
{
    float f32PPMVal;
    uint16_t u16RawVal;

    while (1)
    {
        if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
        {
            /* Simulate BME680 sensor values */
            str_global_sensor_data.bme680.f32Temperature = 20.0f + (esp_random() % 2000) / 100.0f;  /* 20°C to 40°C */
            str_global_sensor_data.bme680.f32Humidity = 30.0f + (esp_random() % 4000) / 100.0f;     /* 30% to 70% */

            /* Keep time update logic */
            if (isNTPSynched)
            {
                update_current_time();
            }
            else
            {
                get_time();
            }

            /* Simulate MQ135 sensor (Gas sensor) */
            f32PPMVal = 200.0f + (esp_random() % 8000) / 10.0f;  /* 200 to 1000 PPM */
            str_global_sensor_data.bme680.u16GasRes = f32PPMVal;

            /* Simulate SDS011 (PM2.5 and PM10) */
            uint16_t u16ScaledPM25 = (uint16_t)(esp_random() % 5000); /* PM2.5 (0 to 500, scaled by 10) */
            uint16_t u16ScaledPM10 = (uint16_t)(esp_random() % 5000); /* PM10 (0 to 500, scaled by 10) */

            str_global_sensor_data.sds011.u16PM25Val = u16ScaledPM25;
            str_global_sensor_data.sds011.u16PM10Val = u16ScaledPM10;

            /* Update Air Quality Index (AQI) */
            str_global_sensor_data.air_quality_index = get_aqi(u16ScaledPM25 / 10.0, u16ScaledPM10 / 10.0);


            // control_rgb_led(str_global_sensor_data.air_quality_index);

            /* Release Semaphore for other tasks */
            xSemaphoreGive(dataSyncSemaphore);
        }

        vTaskDelay(800 / portTICK_PERIOD_MS);
    }
}


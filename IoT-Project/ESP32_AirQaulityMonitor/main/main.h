#ifndef MAIN_H
#define MAIN_H

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include "freertos/event_groups.h"

/* Project-specific includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <esp_flash.h>
#include <esp_err.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "rtc.h"
#include "bme680.h"
#include "bme680_sensor.h"
#include "wifi.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "ble.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "mqtts.h"
#include "sds011.h"
#include "adc.h"

/* Define Macros */
#define SELECT_FACTORY_MODE            	0x01

#define WIFI_PRIVISION          	0

#define ADDR_CONFIG_DATA       		0x0100  /* Configuration data (16 bytes) */

#define MQ135_PIN GPIO_NUM_36

#pragma pack(push, 1)

extern int raw_value;

/* BME680 sensor data collection structure */

typedef struct
{
    float f32Temperature;   	/* 4 bytes 	*/
    float f32Humidity;      	/* 4 bytes	*/
    uint16_t u16GasRes; 	/* Reduced size (e.g., resolution < 1 ohm unnecessary) */
}STR_BME680_SENSOR_DATA;

/* Common structure created for all sensors */

typedef struct
{
    STR_BME680_SENSOR_DATA bme680;          	/* 10 bytes */
    STR_SDS011_SENSOR_DATA sds011;          	/* 4 bytes */
    uint16_t air_quality_index;    	/* 2 bytes, supports AQI up to 500 */
    char atime_str[20];             	/* Compact ISO format "HH:MM:SS" */
    char alocation[16];           	/* MAC-like binary ID */
}STR_SENSOR_DATA;


void rgb_led_init(void);

void control_rgb_led(int aqi);


/* Sempaphore */
extern SemaphoreHandle_t dataSyncSemaphore;

/* Declare shared sensor data */
extern STR_SENSOR_DATA global_sensor_data;

extern STR_SENSOR_DATA processed_data;

extern EventGroupHandle_t wifi_event_group;


/* Function prototypes */
void dataCollectionTask(void *pvParameters);

void data_processing_task(void *pvParameters);

void dataDisplayTask(void *pvParameters);

void dataStorageTask(void *pvParameters);

void dataSendCloudTask(void *pvParameters);

void command_receive_task(void *pvParameters);

void sds011_task(void *pvParameters);

void mq_135(void);

void publish_sensor_data(STR_SENSOR_DATA *data, esp_mqtt_client_handle_t client);

#endif  /* MAIN_H */



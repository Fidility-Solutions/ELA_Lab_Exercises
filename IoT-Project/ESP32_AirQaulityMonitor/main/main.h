#ifndef MAIN_H
#define MAIN_H

/* FreeRTOS includes */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

/* Project-specific includes */
/* Include Header files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <esp_flash.h>
#include <esp_err.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"
#include "mqtt_client.h"
#include "rtc.h"
#include "bme680.h"
#include "bme680_sensor.h"
#include "eeprom.h"
#include "wifi.h"
#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "ble.h"
#include "stdlib.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "mqtts.h"
#include "sds011.h"
#include "adc.h"

/* Define Macros */
#define FACTORY_MODE            1

#define WIFI_PRIVISION          0

#define ADDR_CONFIG_DATA        0x0100  /* Configuration data (16 bytes) */

extern int raw_value;


typedef struct
{
    float temperature;
    float humidity;
    float pressure;
    float gas_resistance;
    bool bme680_valid;
} bme680_data_t;
//
//typedef struct
//{
//    float pm25;
//    float pm10;
//    bool sds011_valid;
//} sds011_data_t;

typedef struct
{
    bme680_data_t bme680;
    sds011_data_t sds011;
    int air_quality_index;  /* Calculated AQI */
    char time_str[20];
} sensor_data_t;


/* Declare global queues and mutex */
extern QueueHandle_t sensorDataQueue;
extern QueueHandle_t processedDataQueue;
extern SemaphoreHandle_t dataSyncSemaphore;

/* Declare shared sensor data */
extern sensor_data_t global_sensor_data;
extern sensor_data_t processed_data;

extern EventGroupHandle_t wifi_event_group;

//xtern nvs_handle_t nvs_handle;

/* Function prototypes */
void dataCollectionTask(void *pvParameters);
void data_processing_task(void *pvParameters);
void dataDisplayTask(void *pvParameters);
void dataStorageTask(void *pvParameters);
void data_send_task(void *pvParameters);
void command_receive_task(void *pvParameters);
void sds011_task(void *pvParameters);



#endif



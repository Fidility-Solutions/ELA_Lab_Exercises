#ifndef MAIN_H
#define MAIN_H

/* FreeRTOS includes */
/* Project-specific includes */
#include "mqtt_client.h"
#include "wifi.h"
#include "sds011.h"
#include "adc.h"

/* Define Macros */
#define SELECT_OPERATIONAL_MODE            	0x01

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
    STR_BME680_SENSOR_DATA bme680;       /*   	10 bytes 	*/
    STR_SDS011_SENSOR_DATA sds011;       /* 	4 bytes 	*/
    uint16_t air_quality_index;          /* 	2 bytes		*/
    char atime_str[20];                  /* 	Compact ISO format "HH:MM:SS" */
    char alocation[32];                  /* 	Location string 	*/
    char as8DeviceId[32];                /* 	Device ID 		*/
    char ps8HVersion[16];                /* 	Hardware version 	*/
    char ps8SVersion[16];                /* 	Software version 	*/
} STR_SENSOR_DATA;


void rgb_led_init(void);

void control_rgb_led(int aqi);


/* Sempaphore */
extern SemaphoreHandle_t dataSyncSemaphore;

/* Declare shared sensor data */
extern STR_SENSOR_DATA str_global_sensor_data;

extern STR_SENSOR_DATA processed_data;

extern EventGroupHandle_t wifi_event_group;


/* Function prototypes */
void data_processing_task(void *pvParameters);

void command_receive_task(void *pvParameters);

void sds011_task(void *pvParameters);

void mq_135(void);

void publish_sensor_data(STR_SENSOR_DATA *data, esp_mqtt_client_handle_t client);

#endif  /* MAIN_H */



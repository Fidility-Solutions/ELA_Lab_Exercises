#ifndef SDS011_H
#define SDS011_H

#pragma once

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#define BUF_SIZE 		256
#define SDS011_TAG 		"SDS011"
#define SDS011_UART_NUM 	UART_NUM_1
#define SDS011_UART_TX_PIN     	17 
#define SDS011_UART_RX_PIN     	16
#define MAX_AQI_VALUE		500
#define UART_BUARD_RATE		9600
#pragma pack(push, 1)



/* SDS011 Sensor Data Structure */
typedef struct
{
    uint16_t u16PM25Val;           /* Scaled to store ppm or ppb values (e.g., 55.8 -> 558) */
    uint16_t u16PM10Val;           /* Same as pm25 */
}STR_SDS011_SENSOR_DATA;


/* AQI Breakpoints (India CPCB) */
typedef struct
{
        float f32CLowVal, f32CHighVal;
        uint32_t u32ILowVal, u32IHighVal;
} STR_AQI_BREAKPOINT;


/* Function to initialize SDS011 sensor */
esp_err_t sds011_init(void);

/* Function to read PM2.5 and PM10 values */
esp_err_t sds011_read(STR_SDS011_SENSOR_DATA *pstr_sds011_sensordata);
uint16_t get_aqi(float f32PM25Val, float f32PM10Val);

#endif /* SDS011_H */


#ifndef SDS011_H
#define SDS011_H

#include "driver/uart.h"
#include "esp_err.h"
#include "esp_log.h"

#define BUF_SIZE 		256
#define SDS011_TAG 			"SDS011"
#define SDS011_UART_NUM 		UART_NUM_1
#define SDS011_UART_TX_PIN     17 
#define SDS011_UART_RX_PIN     16


/* SDS011 Sensor Data Structure */
typedef struct 
{
    	float pm25;
    	float pm10;
	bool sds011_valid;
} sds011_data_t;

/* AQI Breakpoints (India CPCB) */
typedef struct
{
        float C_low, C_high;
        int I_low, I_high;
} aqi_breakpoint_t;


/* Function to initialize SDS011 sensor */
esp_err_t sds011_init(void);

/* Function to read PM2.5 and PM10 values */
esp_err_t sds011_read(sds011_data_t *sensor);
int get_aqi(float pm25, float pm10);

#endif /* SDS011_H */


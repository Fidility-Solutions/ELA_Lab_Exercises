#include <stdio.h>
#include "main.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <bme680.h>
#include <string.h>
#include "driver/adc.h"
#include "bme680_sensor.h"
#include "esp_adc/adc_oneshot.h"
#include <math.h>
#include "sds011.h"

float calculate_ppm(int adc_value);

extern int raw_value;
extern bme680_t sensor;  /* Global sensor object */
extern bme680_values_float_t values;  /* Global sensor values */
extern uint32_t duration;
int raw_value;
extern SemaphoreHandle_t dataSyncSemaphore;

//sensor_data_t global_sensor_data;

extern sds011_data_t sds011_data;



#define MQ135_PIN GPIO_NUM_36  // Example, replace with the correct pin number

#define TAG "ADC"

void dataCollectionTask(void *pvParameters)
{
	esp_err_t err;
	float ppm;
	while (1)
	{
		if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
		{
			/* trigger the sensor to start one TPHG measurement cycle */
			if (bme680_force_measurement(&sensor) == ESP_OK)
			{
				/*  passive waiting until measurement results are available */
				vTaskDelay(duration);
				/* get the results and do something with them */
				if (bme680_get_results_float(&sensor, &values) == ESP_OK)
				{
					get_time();
					global_sensor_data.bme680.temperature = values.temperature;
					global_sensor_data.bme680.humidity = values.humidity;
				}
			}

			int adc_value = 0;
			float adc_value1 = 0.0;
			// Read MQ135 sensor (quick operation)
			raw_value = read_mq135();
			ppm = calculate_mq135_ppm();
			global_sensor_data.bme680.gas_resistance = ppm;



			/* Read SDS011 sensor (PM2.5 & PM10) */
			err = sds011_read(&sds011_data);
			//if(err != ESP_OK) {
			//	ESP_LOGE("DataCollection", "Failed to read SDS011 sensor: %s",
			//			esp_err_to_name(err));
			//	// Don't return error, continue with partial data
			//}

			global_sensor_data.air_quality_index = get_aqi(sds011_data.pm25, sds011_data.pm10);



			if (sensorDataQueue != NULL)
			{ 
				if (xQueueOverwrite(sensorDataQueue, &global_sensor_data) != pdPASS) {
					ESP_LOGI("SensorTask", "Failed overwrote data in the queue");}
			}
			/* Collection sensors data */
			xSemaphoreGive(dataSyncSemaphore);

		}
		else {
			ESP_LOGE("MUTEX", "Failed to take mutex, retrying...");
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

//float calculate_ppm(int adc_value)
//{
//
//	float voltage = (adc_value * 3.3) / 4095.0;
//	float rs = RL_VALUE * ((5.0 / voltage) - 1.0);
//	float ppm = A * pow((rs / R0), B);
//	return ppm;
//}


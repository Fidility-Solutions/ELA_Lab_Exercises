/* Include Header files */
#include "main.h"

extern bme680_t sensor;  		/* Global sensor object */

extern bme680_values_float_t values;  	/* Global sensor values */

extern STR_SDS011_SENSOR_DATA str_sds011_sensor_data;       /* Global sensor values */

extern uint32_t duration;

int raw_value;

/**
 * This file contains functions for reading, processing, and storing

 * sensor data from the BME680 sensor(Temperature and humidity), pm sensor(AQI value),
 
 * mq135 sensor(CO2) to the global structure.
 
 */

void dataCollectionTask(void *pvParameters)
{
	float ppm;
	while (1)
	{
		if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
		{
			/* Trigger the sensor to start one TPHG measurement cycle */
			if (bme680_force_measurement(&sensor) == ESP_OK)
			{
				/* Passive waiting until measurement results are available */
				vTaskDelay(duration);

				/* Get the results and do something with them */
				if (bme680_get_results_float(&sensor, &values) == ESP_OK)
				{
					if (ntp_synchronized) {
						update_current_time();
					} else {
						get_time();
					}
					global_sensor_data.bme680.f32Temperature = values.temperature;
					global_sensor_data.bme680.f32Humidity = values.humidity;
				}
			}

			/* Read MQ135 sensor (quick operation) */
			raw_value = read_mq135();
			ppm = calculate_mq135_ppm();
			global_sensor_data.bme680.u16GasRes = ppm;



			/* Read SDS011 sensor (PM2.5 & PM10) */
			sds011_read(&str_sds011_sensor_data);

			uint16_t scaled_pm25 = (uint16_t)(str_sds011_sensor_data.u16PM25Val * 10); // Scale PM2.5 by 10
			uint16_t scaled_pm10 = (uint16_t)(str_sds011_sensor_data.u16PM10Val * 10); // Scale PM10 by 10


			global_sensor_data.sds011.u16PM25Val = scaled_pm25;
			global_sensor_data.sds011.u16PM10Val = scaled_pm10;


			/* Update Air Quality Index (AQI) */
			global_sensor_data.air_quality_index = get_aqi(scaled_pm25 / 10.0, scaled_pm10 / 10.0); // Unscale to compute AQI
														
			strcpy(global_sensor_data.alocation, "Electronic City");
		//	control_rgb_led(global_sensor_data.air_quality_index);

			/* Release Semaphore for other tasks */
			xSemaphoreGive(dataSyncSemaphore);
		}

		vTaskDelay(800 / portTICK_PERIOD_MS);
	}
}


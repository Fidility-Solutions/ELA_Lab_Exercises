//#include <stdio.h>
//#include <freertos/FreeRTOS.h>
//#include "freertos/task.h"
//#include <esp_system.h>
#include <string.h>
//#include "driver/i2c.h"
#include "driver/i2c_master.h"
//#include "esp_log.h"
#include <bme680.h>
#include "bme680_sensor.h"
#include "main.h"

#define PORT 0
//#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_0)
//#define ADDR_BME680 BME680_I2C_ADDR_0
#define ADDR_BME680 0x76

//#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_0)
//#define ADDR_BME680 BME680_I2C_ADDR_0
//#endif
//#if defined(CONFIG_EXAMPLE_I2C_ADDRESS_1)
//#define ADDR_BME680 BME680_I2C_ADDR_1
//#endif

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif

#define CONFIG_EXAMPLE_I2C_MASTER_SDA   21   // Change to your SDA pin
#define CONFIG_EXAMPLE_I2C_MASTER_SCL   22   // Change to your SCL pin
//NOTE : Needs to be configured later


bme680_t sensor;  // Global sensor object
uint32_t duration;  // Global duration for the measurement cycle
bme680_values_float_t values;  // Global sensor values


void BME680_task_start(void)
{
	ESP_ERROR_CHECK(i2cdev_init());

	memset(&sensor, 0, sizeof(bme680_t));

	ESP_ERROR_CHECK(bme680_init_desc(&sensor, ADDR_BME680, PORT, CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));

	// init the sensor
	ESP_ERROR_CHECK(bme680_init_sensor(&sensor));

	// Changes the oversampling rates to 4x oversampling for temperature
	// and 2x oversampling for humidity and Pressure measurement.
	bme680_set_oversampling_rates(&sensor, BME680_OSR_4X, BME680_OSR_2X, BME680_OSR_2X);

	// Change the IIR filter size for temperature and pressure to 7.
	bme680_set_filter_size(&sensor, BME680_IIR_SIZE_7);

	// Change the heater profile 0 to 200 degree Celsius for 100 ms.
	bme680_set_heater_profile(&sensor, 0, 200, 100);
	bme680_use_heater_profile(&sensor, 0);

	// Set ambient temperature to 10 degree Celsius
	bme680_set_ambient_temperature(&sensor, 10);

	// as long as sensor configuration isn't changed, duration is constant
//	uint32_t duration;
	bme680_get_measurement_duration(&sensor, &duration);

	//TickType_t last_wakeup = xTaskGetTickCount();
}

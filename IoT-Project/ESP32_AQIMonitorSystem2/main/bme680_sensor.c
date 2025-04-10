/******************************************************************************
 * File         : bme680_sensor.c
 *
 * Description  : This file initializes the BME680 environmental sensor over I2C 
 *                and sets up various sensor configurations like oversampling,
 *                filtering, and gas heater profile.
 *
 * Author       : Fidility Solutions.
 *
 * Reference    : BME680 Datasheet and ESP-IDF BME680 Driver Documentation.
 *******************************************************************************/


/* Include Header files */
#include "bme680_sensor.h"
#include "bme680.h"
//#include "main.h"
#include <string.h>

bme680_t sensor;  		/* Global sensor object */
uint32_t duration;  		/* Global duration for the measurement cycle */
bme680_values_float_t values;  	/* Global sensor values  */



/*
 * Function     : BME680_task_start()
 *
 * Description  : This function initializes the I2C driver and the BME680 sensor,
 *                configures oversampling rates for temperature, pressure, and humidity,
 *                sets the IIR filter size, configures the gas sensor's heater profile,
 *                and determines the measurement duration based on the settings.
 *
 * Parameters   : None
 * 
 * Returns      : None
 *
 */
void BME680_task_start(void)
{
	ESP_ERROR_CHECK(i2cdev_init());

	memset(&sensor, 0, sizeof(bme680_t));

	ESP_ERROR_CHECK(bme680_init_desc(&sensor, ADDR_BME680, PORT, CONFIG_EXAMPLE_I2C_MASTER_SDA, 
				CONFIG_EXAMPLE_I2C_MASTER_SCL));

	/* init the sensor */
	ESP_ERROR_CHECK(bme680_init_sensor(&sensor));

	/* Changes the oversampling rates to 4x oversampling for temperature */
	/* and 2x oversampling for humidity and Pressure measurement. */
	bme680_set_oversampling_rates(&sensor, BME680_OSR_4X, BME680_OSR_2X, BME680_OSR_2X);

	/* Change the IIR filter size for temperature and pressure to 7. */
	bme680_set_filter_size(&sensor, BME680_IIR_SIZE_7);

	/* Change the heater profile 0 to 200 degree Celsius for 100 ms. */
	bme680_set_heater_profile(&sensor, 0, 200, 100);
	bme680_use_heater_profile(&sensor, 0);

	/* Set ambient temperature to 10 degree Celsius */
	bme680_set_ambient_temperature(&sensor, 10);

	/* as long as sensor configuration isn't changed, duration is constant */
	bme680_get_measurement_duration(&sensor, &duration);

}

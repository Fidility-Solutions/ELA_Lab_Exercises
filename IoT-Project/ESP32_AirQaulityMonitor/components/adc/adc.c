/*******************************************************************************
 * File        : adc.c
 *
 * Description : This file provides the implementation for interfacing with the MQ-135 gas sensor
 *               using the ADC peripheral of the ESP32. It includes functions to initialize the ADC,
 *               read analog values from the sensor, and convert those values into PPM (parts per million)
 *               using a predefined gas concentration formula.
 *
 * Author      : Fidility Solutions.
 *
 * Reference   : MQ-135 Datasheet, ESP-IDF ADC API Documentation.
 * 		https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/adc_oneshot.html
 *
 *******************************************************************************/
/* Include header file */
#include "adc.h"
#include <math.h>
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "driver/adc.h"
#include "esp_log.h"
static adc_oneshot_unit_handle_t adc_handle;


/*******************************************************************************
 * Function:    calculate_mq135_ppm
 *
 * Description:
 *      Calculates the PPM (parts per million) value from the MQ135 gas sensor
 *      based on the raw ADC value read. The function converts the ADC value 
 *      to voltage, calculates the sensor resistance (Rs), and then applies the 
 *      logarithmic MQ135 formula to estimate gas concentration.
 *
 * Returns:
 *      float - Estimated gas concentration in PPM.
 *
 * Notes:
 *      - Ensure `raw_value`, `RL_VALUE`, `A`, `B`, and `R0` are properly defined globally.
 *      - `R0` is the resistance of the sensor in clean air (calibrated baseline).
 *      - Voltage reference assumed to be 3.3V.
 *******************************************************************************/
float calculate_mq135_ppm(uint16_t u32rawVal)
{

	float f32Voltage = (u32rawVal * 3.3) / 4095.0;
	float f32SensorRes = MQ135_LOAD_RESISTANCE_KOHM * ((5.0 / f32Voltage) - 1.0);
	float f32PPMVal = MQ135_CO2_CURVE_CONSTANT * pow((f32SensorRes / MQ135_CLEAN_AIR_RESISTANCE_KOHM), 
			MQ135_CO2_CURVE_EXPONENT);

	return f32PPMVal;
}

/*
 * Function    : read_mq135()
 *
 * Description : Reads the analog value from the MQ-135 gas sensor using ADC (ADC1_CHANNEL_0).
 *               The function uses one-shot ADC read to capture the sensor's output voltage.
 *
 * Parameters  : None
 *
 * Returns     : int32_t - Returns the raw ADC value read from the MQ-135 sensor.
 */
int read_mq135(void)
{

	int s32ADCVal = 0;

	/* Read the analog value from MQ135 sensor connected to ADC1 Channel 0 */
	adc_oneshot_read(adc_handle, ADC1_CHANNEL_0, &s32ADCVal);

	return s32ADCVal;
}

/*
 * Function    : initialize_mq135_adc()
 *
 * Description : Initializes the ADC (Analog-to-Digital Converter) for reading analog values from the MQ-135 gas sensor.
 *               Configures ADC Unit 1 and sets up channel 0 with the default bit width and 12 dB attenuation for accurate voltage measurement.
 *
 * Parameters  : None
 *
 * Returns     : None
 *
 * Notes       : Logs error messages if ADC unit or channel configuration fails.
 *               Should be called once before attempting to read values using the MQ-135 sensor.
 */
void initialize_mq135_adc(void)
{
	esp_err_t eErrStat;

	/* ADC One-Shot Configuration */
	adc_oneshot_unit_init_cfg_t init_config =
	{
		.unit_id = ADC_UNIT_1
	};
	eErrStat = adc_oneshot_new_unit(&init_config, &adc_handle);
	if (eErrStat != ESP_OK) 
	{
		ESP_LOGE("ADC", "Failed to initialize ADC unit: %s", esp_err_to_name(eErrStat));
		return;
	}


	adc_oneshot_chan_cfg_t channel_config =
	{
		.bitwidth = ADC_BITWIDTH_DEFAULT,
		.atten = ADC_ATTEN_DB_12  /* Use directly (no `&`) & */
	};

	eErrStat = adc_oneshot_config_channel(adc_handle, ADC1_CHANNEL_0, &channel_config);
	if (eErrStat != ESP_OK) 
	{
		ESP_LOGE("ADC", "Failed to configure ADC channel: %s", esp_err_to_name(eErrStat));
		return;
	}
	ESP_LOGI("ADC", "ADC initialized successfully");

}

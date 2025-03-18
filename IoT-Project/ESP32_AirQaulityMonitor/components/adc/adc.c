/* Include header file */
#include "adc.h"

/* Reference :https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/peripherals/adc_oneshot.html */
static adc_oneshot_unit_handle_t adc_handle;  // Declare adc_handle globally

/* Calculate PPM */
float calculate_mq135_ppm(void)
{

	float voltage = (raw_value * 3.3) / 4095.0;
	float rs = RL_VALUE * ((5.0 / voltage) - 1.0);
	float ppm = A * pow((rs / R0), B);

	return ppm;
}
/* Read from MQ-135 */
int read_mq135(void)
{

	int adc_value = 0;

	adc_oneshot_read(adc_handle, ADC1_CHANNEL_0, &adc_value);
	printf("ADC:raw value:%d\n",adc_value);

	return adc_value;
}

/* Initialize ADC for MQ-135 */
void initialize_mq135_adc(void)
{
	esp_err_t ret;

	/* ADC One-Shot Configuration */
	adc_oneshot_unit_init_cfg_t init_config =
	{
		.unit_id = ADC_UNIT_1
	};
	ret = adc_oneshot_new_unit(&init_config, &adc_handle);
	if (ret != ESP_OK) 
	{
		ESP_LOGE("ADC", "Failed to initialize ADC unit: %s", esp_err_to_name(ret));
		return;
	}


	adc_oneshot_chan_cfg_t channel_config =
	{
		.bitwidth = ADC_BITWIDTH_DEFAULT,
		.atten = ADC_ATTEN_DB_12  /* Use directly (no `&`) & */
	};
	ret = adc_oneshot_config_channel(adc_handle, ADC1_CHANNEL_0, &channel_config);
	if (ret != ESP_OK) {
		ESP_LOGE("ADC", "Failed to configure ADC channel: %s", esp_err_to_name(ret));
		return;
	}
	ESP_LOGI("ADC", "ADC initialized successfully");

}

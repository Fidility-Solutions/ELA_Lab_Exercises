/* Include Header files */
#include "sds011.h"
sds011_data_t sds011_data = {0, 0};
aqi_breakpoint_t pm25_breakpoints[] ={
    {0, 30, 0, 50}, {31, 60, 51, 100}, {61, 90, 101, 200},
    {91, 120, 201, 300}, {121, 250, 301, 400}, {251, 500, 401, 500}};

aqi_breakpoint_t pm10_breakpoints[] ={
    {0, 50, 0, 50}, {51, 100, 51, 100}, {101, 250, 101, 200},
    {251, 350, 201, 300}, {351, 430, 301, 400}, {431, 500, 401, 500}};

/* Function to calculate AQI using linear interpolation */
int calculate_aqi(float pm, aqi_breakpoint_t breakpoints[], int size) 
{
    	for (int i = 0; i < size; i++) {
        	if (pm >= breakpoints[i].C_low && pm <= breakpoints[i].C_high) {
            		return ((breakpoints[i].I_high - breakpoints[i].I_low) /
                    		(breakpoints[i].C_high - breakpoints[i].C_low)) *
                   		(pm - breakpoints[i].C_low) + breakpoints[i].I_low;
        	}
    	}
    	
	return 500; /* Max AQI if PM is very high */
}

/* Get final AQI */
int get_aqi(float pm25, float pm10) 
{
    	int aqi_pm25 = calculate_aqi(pm25, pm25_breakpoints, 6);
    	int aqi_pm10 = calculate_aqi(pm10, pm10_breakpoints, 6);
    	return (aqi_pm25 > aqi_pm10) ? aqi_pm25 : aqi_pm10;
}

/* Initialize SDS011 UART */
esp_err_t sds011_init(void)
{
	printf("This is sds011 diriver\n");
    	uart_config_t uart_config = {
        	.baud_rate = 9600,
        	.data_bits = UART_DATA_8_BITS,
        	.parity = UART_PARITY_DISABLE,
        	.stop_bits = UART_STOP_BITS_1,
        	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    	};
	 esp_err_t err;
	 printf("this is uart \n");
	err = uart_driver_install(SDS011_UART_NUM, BUF_SIZE, 0, 0, NULL, 0);
    	if (err != ESP_OK) {
        	ESP_LOGE(SDS011_TAG, "uart_driver_install failed: %s", esp_err_to_name(err));
        	return err;
    	}

    	err = uart_param_config(SDS011_UART_NUM, &uart_config);
    	if (err != ESP_OK) {
        	ESP_LOGE(SDS011_TAG, "uart_param_config failed: %s", esp_err_to_name(err));
        	return err;
    	}

    	err = uart_set_pin(SDS011_UART_NUM, SDS011_UART_TX_PIN, SDS011_UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    	if (err != ESP_OK) {
        	ESP_LOGE(SDS011_TAG, "uart_set_pin failed: %s", esp_err_to_name(err));
        	return err;
    	}

    	ESP_LOGI(SDS011_TAG, "SDS011 initialized");
    	return ESP_OK;

}

/* Read SDS011 sensor data */
esp_err_t sds011_read(sds011_data_t *sensor) 
{
    	uint8_t ucbuffer[10];
    	//sds011_data_t sensor_data = {0, 0};

    	//int len = uart_read_bytes(SDS011_UART_NUM, ucbuffer, 10, pdMS_TO_TICKS(500));
	int len = uart_read_bytes(SDS011_UART_NUM, ucbuffer, 10, 0);
    	if (len == 10)// && ucbuffer[0] == 0xAA && ucbuffer[1] == 0xC0 && ucbuffer[9] == 0xAB) 
    	{
        	sensor->pm25 = (ucbuffer[2] | (ucbuffer[3] << 8)) / 10.0;
        	sensor->pm10 = (ucbuffer[4] | (ucbuffer[5] << 8)) / 10.0;
		return ESP_OK;
    	} 
	else 
	{
        	//ESP_LOGE(SDS011_TAG, "Invalid SDS011 data");
		return ESP_FAIL;
    	}
	return ESP_OK;
}


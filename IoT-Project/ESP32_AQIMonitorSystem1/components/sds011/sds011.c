/*******************************************************************************
 * File          : sds011.c
 *
 * Description   : This file provides the implementation for interfacing with the SDS011 
 *                 particulate matter sensor using UART on the ESP32 platform. It includes
 *                 functions for initializing the UART, reading data from the SDS011 sensor,
 *                 and calculating the Air Quality Index (AQI) based on the PM2.5 and PM10
 *                 values using standard AQI breakpoints.
 *
 *                 Key functionalities:
 *                   - UART initialization for SDS011 communication.
 *                   - Extraction of PM2.5 and PM10 data from SDS011 response frames.
 *                   - Calculation of AQI using predefined breakpoint tables.
 *                   - Return of overall AQI based on the worst-case pollutant.
 *
 * Author        : Rajanikanth
 *
 * Date          : 08/04/2025
 *
 * Reference     : 
 *                 - SDS011 Sensor Datasheet by Nova Fitness Co., Ltd.
 *                 - AQI Breakpoint values as per CPCB (India) and US EPA standards.
 *                 - ESP-IDF UART Driver documentation.
 *
 *******************************************************************************/



/* Include Header files */
#include "sds011.h"


STR_SDS011_SENSOR_DATA str_sds011_sensor_data = {0, 0};
STR_AQI_BREAKPOINT str_pm25_breakpoints[] ={
    {0, 30, 0, 50}, {31, 60, 51, 100}, {61, 90, 101, 200},
    {91, 120, 201, 300}, {121, 250, 301, 400}, {251, 500, 401, 500}};

STR_AQI_BREAKPOINT str_pm10_breakpoints[] ={
    {0, 50, 0, 50}, {51, 100, 51, 100}, {101, 250, 101, 200},
    {251, 350, 201, 300}, {351, 430, 301, 400}, {431, 500, 401, 500}};

/*
 * Function     : calculate_aqi()
 *
 * Description  : This function calculates the Air Quality Index (AQI) based on the given PM (Particulate Matter)
 *                concentration value. It iterates through a set of predefined AQI breakpoints to determine which
 *                range the PM value falls into and applies the standard AQI linear interpolation formula to compute
 *                the AQI value.
 *
 * Parameters   :
 *      f32PMVal           - The measured PM value (e.g., PM2.5) for which the AQI is to be calculated.
 *      str_breakpoints[]  - An array of AQI breakpoint structures that define concentration and AQI value ranges.
 *      u16Size            - The number of elements in the breakpoint array.
 *
 * Returns      :
 *      uint16_t - The computed AQI value corresponding to the PM value. If the PM value does not fall within any
 *            defined range, a maximum AQI value (MAX_AQI_VALUE) is returned to indicate extremely poor air quality.
 *
 */
uint16_t calculate_aqi(float f32PMVal, STR_AQI_BREAKPOINT str_breakpoints[], uint16_t u16Size) 
{
    	for (int i = 0; i < u16Size; i++) 
	{
        	if (f32PMVal >= str_breakpoints[i].f32CLowVal && f32PMVal <= str_breakpoints[i].f32CHighVal) 
		{
            		return ((str_breakpoints[i].u32IHighVal - str_breakpoints[i].u32ILowVal) /
                    		(str_breakpoints[i].f32CHighVal - str_breakpoints[i].f32CLowVal)) *
                   		(f32PMVal - str_breakpoints[i].f32CLowVal) + str_breakpoints[i].u32ILowVal;
        	}
    	}
    	
	return MAX_AQI_VALUE; /* Max AQI if PM is very high */
}

/*
 * Function     : get_aqi()
 *
 * Description  : This function calculates the Air Quality Index (AQI) for both PM2.5 and PM10 pollutant levels
 *                using predefined AQI breakpoints. It computes the AQI individually for both PM types and returns 
 *                the higher value, as the overall AQI is determined by the worst-case pollutant.
 *
 * Parameters   : 
 *      f32PM25Val - The measured PM2.5 value (in µg/m³).
 *      f32PM10Val - The measured PM10 value (in µg/m³).
 *
 * Returns      : 
 *      uint16_t - The higher AQI value between PM2.5 and PM10, representing the overall AQI.
 *
 */
uint16_t get_aqi(float f32PM25Val, float f32PM10Val) 
{
    	uint16_t u16PM25Val = calculate_aqi(f32PM25Val, str_pm25_breakpoints, 6);
    	uint16_t u16PM10Val = calculate_aqi(f32PM25Val, str_pm10_breakpoints, 6);
    	
	return (u16PM25Val > u16PM10Val) ? u16PM25Val: u16PM10Val;
}


/*
 * Function     : sds011_init()
 *
 * Description  : This function initializes the UART interface to communicate with the SDS011 air quality sensor. 
 *                It sets up the UART parameters, installs the UART driver, and configures the TX/RX pins.
 *
 * Parameters   : None
 *
 * Returns      : 
 *      esp_err_t - ESP_OK if successful, or appropriate error code in case of failure.
 */
esp_err_t sds011_init(void)
{
	/* Configure UART parameters for SDS011 sensor communication */
    	uart_config_t uart_config = 
	{
        	.baud_rate = UART_BUARD_RATE, 		/* Set baud rate */
        	.data_bits = UART_DATA_8_BITS,		/* 8 data bits */
        	.parity = UART_PARITY_DISABLE,		/* No parity */
        	.stop_bits = UART_STOP_BITS_1,		/* 1 stop bit */
        	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE, 	/* Disable hardware flow control */
    	};
	
	esp_err_t eErrStat;

	/* Install UART driver with RX buffer size and no TX buffer or event queue */
	eErrStat = uart_driver_install(SDS011_UART_NUM, BUF_SIZE, 0, 0, NULL, 0);
    	
	if (eErrStat != ESP_OK) 
	{
        	ESP_LOGE(SDS011_TAG, "uart_driver_install failed: %s", esp_err_to_name(eErrStat));
        	return eErrStat;
    	}

	/* Apply the configured UART parameters */
    	eErrStat = uart_param_config(SDS011_UART_NUM, &uart_config);
    	if (eErrStat != ESP_OK) 
	{
        	ESP_LOGE(SDS011_TAG, "uart_param_config failed: %s", esp_err_to_name(eErrStat));
        	return eErrStat;
    	}

	/* Set the UART TX and RX pins for SDS011 sensor */
    	eErrStat = uart_set_pin(SDS011_UART_NUM, SDS011_UART_TX_PIN, SDS011_UART_RX_PIN, 
			UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    	if (eErrStat != ESP_OK) 
	{
        	ESP_LOGE(SDS011_TAG, "uart_set_pin failed: %s", esp_err_to_name(eErrStat));
        	return eErrStat;
    	}

    	ESP_LOGI(SDS011_TAG, "SDS011 initialized");
    	
	return ESP_OK;

}


/*
 * Function     : sds011_read()
 *
 * Description  : Reads 10 bytes of data from the SDS011 sensor via UART. If the read is successful and the data
 *                format is valid, it extracts PM2.5 and PM10 values from the received bytes and stores them in 
 *                the provided structure.
 *
 * Parameters   : 
 *      pstr_sds011_sensordata - Pointer to a structure where the extracted PM2.5 and PM10 values will be stored.
 *
 * Returns      : 
 *      ESP_OK   - If data was successfully read and parsed.
 *      ESP_FAIL - If the data length is invalid or if the read failed.
 */
esp_err_t sds011_read(STR_SDS011_SENSOR_DATA *pstr_sds011_sensordata) 
{
    	uint8_t aucBuffer[10];

	/* Read 10 bytes from SDS011 sensor over UART (non-blocking) */
	uint8_t ucDataLen = uart_read_bytes(SDS011_UART_NUM, aucBuffer, 10, 0);

	/* Check if full 10-byte packet is received */
	if (ucDataLen == 10 && aucBuffer[0] == 0xAA && aucBuffer[1] == 0xC0 && aucBuffer[9] == 0xAB)
	{
		/* Extract PM2.5 value (little endian: byte[2] + byte[3] << 8), divide by 10 to get µg/m³ */
        	pstr_sds011_sensordata->u16PM25Val = (aucBuffer[2] | (aucBuffer[3] << 8)) / 10.0;
        	
		/* Extract PM10 value (little endian: byte[4] + byte[5] << 8), divide by 10 to get µg/m³ */
		pstr_sds011_sensordata->u16PM10Val = (aucBuffer[4] | (aucBuffer[5] << 8)) / 10.0;
		
		return ESP_OK;
    	} 
	else 
	{
		return ESP_FAIL;
    	}
	
	return ESP_OK;
}


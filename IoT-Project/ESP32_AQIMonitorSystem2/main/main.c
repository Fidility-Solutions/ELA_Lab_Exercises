
/******************************************************************************
 * File         : main.c
 *
 * Description  : This is the main entry point of the Air Quality Monitor System
 *                based on the ESP32 platform. It handles the initialization
 *                of various system components like I2C, RTC, sensors, and EEPROM
 *                and determines whether the system should operate in factory mode
 *                or operational mode.
 *                
 *                - The system starts by initializing the NVS (Non-Volatile Storage),
 *                  which is used for saving configurations or flags that need to persist
 *                  across reboots.
 *                - It checks the EEPROM to determine the system's mode (factory or operational).
 *                - Based on this mode, the system either enters factory mode for setup
 *                  or operational mode to start its normal operations.
 *                - In factory mode, the system performs setup operations and then restarts.
 *                - In operational mode, it performs continuous operations like data
 *                  collection, storage, display, and cloud communication.
 *                
 * Author       : FidilitySolutions
 *
 * Parameters   : None
 *
 * Return       : None
 ******************************************************************************/

/* Header files */
#include "main.h"
#include "eeprom.h"
#include "data_display.h"
#include "data_collection.h"
#include "data_storage.h"
#include <nvs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <nvs_flash.h>
#include <esp_flash.h>
#include <esp_err.h>
#include "mqtts.h"
#include "ble.h"
#include "bme680_sensor.h"
#include "rtc.h"
#include "spi.h"
#include "esp_heap_caps.h"
#include <inttypes.h>
#include "event_manager.h"

/* global sensor data */
SemaphoreHandle_t dataSyncSemaphore = NULL;

EventGroupHandle_t xEventGroup;

extern uint8_t u8CloudConnect;


/* Static functions */
static void init_nvs(void);

static void factor_mode(void);

static void operational_mode(void);


static void driver_init(void);

static void wifi_establish(void);

static void read_and_print_device_info(void);

/******************************************************************************
 * Function     : app_main
 *
 * Description  : This is the main application entry point. It is responsible for
 *                initializing system components and determining the mode of the
 *                system (factory or operational mode). It performs the following steps:
 *                - Initializes NVS (Non-Volatile Storage).
 *                - Creates an event group to enable communication between tasks.
 *                - Initializes I2C for EEPROM and RTC (Real-Time Clock).
 *                - Initializes various drivers required for the system (e.g., sensors).
 *                - Checks the factory mode flag in EEPROM to determine whether the system
 *                  should run in factory mode or operational mode.
 *                - If in factory mode, it writes to the EEPROM and restarts the system.
 *                - If in operational mode, it proceeds with normal system operation.
 *                - Based on the mode, it either enters `factor_mode()` or `operational_mode()`.
 *
 * Author       : [Your Name or Company]
 *
 * Parameters   : None
 *
 * Return       : None
 ******************************************************************************/
void app_main(void)
{
	/* Avoid Priting Unused logs */
	esp_log_level_set("gpio", ESP_LOG_ERROR);
	esp_log_level_set("EEPROM_DRIVER",ESP_LOG_ERROR);
	esp_log_level_set("wifi",ESP_LOG_ERROR);
	esp_log_level_set("wifi_init",ESP_LOG_ERROR);
	esp_log_level_set("mqtt_client",ESP_LOG_ERROR);
	
	uint8_t u8Mode = 0x01;
	uint8_t factory_mode = 0;

	/* Initialize NVS */
	init_nvs();

	/* Create Event Group for multiple tasks to communicate based on shared event flags */
	xEventGroup = xEventGroupCreate();

	/* Initialize I2C for EEPROM */
	init_RTC();

	/* Initialize the Drivers */
	driver_init();

	factory_mode = eeprom_read_byte(FACTORY_MODE_FLAG_ADDR);
	printf("factor mode = %x\n", factory_mode);

	if(factory_mode != SELECT_OPERATIONAL_MODE)
	{
		if (eeprom_write(FACTORY_MODE_FLAG_ADDR, &u8Mode, 1) != ESP_OK)
		{
			ESP_LOGI("MAIN", "EEPROM Write Failed");
		}

		factor_mode();
	}
	else
	{
		operational_mode();
	}
}

/******************************************************************************
 * Function     : init_nvs
 *
 * Description  : This function initializes the Non-Volatile Storage (NVS) of the ESP32.
 *                It checks if the NVS flash initialization is successful. If the 
 *                initialization fails due to a lack of free pages or a new NVS version 
 *                being found, it erases the NVS flash and reinitializes it to ensure 
 *                proper operation.
 *
 * Parameters   : None
 *
 * Return       : None
 ******************************************************************************/
static void init_nvs(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		nvs_flash_erase();
		nvs_flash_init();
	}
}




/******************************************************************************
 * Function     : factor_mode
 *
 * Description  : This function handles the main operation flow of a device
 *                when it is in "factory mode" or initial setup mode. It
 *                initializes the BLE (Bluetooth Low Energy) and Wi-Fi, waits
 *                for specific event flags, and connects to both the Wi-Fi
 *                network and an MQTT cloud server. The function will handle
 *                reconnection attempts and ensure the device is ready for further
 *                operations. If the MQTT connection is successful, the device
 *                will restart as part of the connection process.
 *
 * Arguments    : None
 *
 * Return       : None
 *
 * Purpose      : The main goal of this function is to manage the device's
 *                connection to Wi-Fi and MQTT cloud services, making it suitable
 *                for use during factory setup or initial configuration. Once
 *                Wi-Fi and MQTT cloud connection are successful, the device
 *                restarts as part of the initialization process.
 *****************************************************************************/
#ifndef MANUAL_CONFIG
static void factor_mode(void)
{
	/* Initialize BLE */
	ble_init();

	while(1)
	{
		/* Wait for events (Wi-Fi setup or reconnection) */
		EventBits_t bits = xEventGroupWaitBits(xEventGroup,
				BLE_WIFI_CONNECT_BIT | WIFI_CONNECTED_BIT,
				pdFALSE, pdFALSE, portMAX_DELAY);

		if ((bits & BLE_WIFI_CONNECT_BIT))
		{
			/* Initialize wifi and establish connection */
			wifi_establish();
			xEventGroupClearBits(xEventGroup, BLE_WIFI_CONNECT_BIT);

		}
		if((bits & WIFI_CONNECTED_BIT))
		{
			/* Connect to MQTT cloud */
			mqtt_app_start();
			vTaskDelay(7000 / portTICK_PERIOD_MS);

			xEventGroupClearBits(xEventGroup, WIFI_CONNECTED_BIT);
			if(u8CloudConnect)
			{
				esp_restart();
			}
			break;

		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

}
#else
static void factor_mode(void)
{
	/* Manual test */
	char ssid[WIFI_SSID_MAX_LEN] = "FidilitySolutions", password[WIFI_PASS_MAX_LEN] = "fidility@123"; 
	while(1)
	{
		/* Manual test */
		wifi_init(ssid,password);

		vTaskDelay(5000 / portTICK_PERIOD_MS);

		mqtt_app_start();
		vTaskDelay(7000 / portTICK_PERIOD_MS);

	}

}
#endif


/******************************************************************************
 * Function     : driver_init
 *
 * Description  : This function performs the initialization of various hardware
 *                components and drivers required for the system's operation.
 *                It performs the following tasks:
 *                1. Initializes the BME680 sensor using the `BME680_task_start()` function.
 *                2. Checks if the EEPROM is connected. If not, it logs an error message
 *                   and restarts the system.
 *                3. Initializes the SDS011 sensor for particulate matter detection using
 *                   the `sds011_init()` function.
 *                4. Initializes the RTC (Real-Time Clock) for timestamp management.
 *                5. Configures the ADC (Analog-to-Digital Converter) for one-shot readings
 *                   of the MQ-135 gas sensor using `initialize_mq135_adc()`.
 *                6. Initializes the LCD display using the `lcd_init()` function.
 *                7. Initializes the RGB LED for status indication using the `rgb_led_init()` function.
 *                8. Logs a message to indicate that the driver initialization is complete.
 *
 * Parameters   : None
 *
 * Return       : None
 ******************************************************************************/
static void driver_init(void)
{
	esp_err_t eErrStat;
	ESP_LOGI("DEBUG", "Free heap before driver init: %" PRIu32, esp_get_free_heap_size());

	/* Initialze I2C driver & bme680 sensor configurations */
	BME680_task_start();

	if(!detect_eeprom())
	{
		ESP_LOGE("EEPROM", "EEPROM not detected. Please check connections");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		esp_restart();
	}

	/* Initialize UART configuration for PM sensor */
	sds011_init();


	/* Initialize RTC for timestamp */

	/* ADC One-Shot Configuration */
	initialize_mq135_adc();

	/* Initilaize SPI driver */
	eErrStat = SPI_Init();
	if (eErrStat != ESP_OK)
	{
		ESP_LOGE(SPI_TAG, "SPI initialization failed");
		return;
	}


	lcd_init();

	rgb_led_init();

	printf("Driver initialize completed\n");
	ESP_LOGI("DEBUG", "Free heap after driver init: %" PRIu32, esp_get_free_heap_size());

}

/******************************************************************************
 * Function     : operational_mode
 *
 * Description  : This function initializes the system for normal operation.
 *                It sets up the Wi-Fi connection, initializes drivers (such as
 *                NTP for time synchronization), connects to the cloud via MQTT,
 *                and creates FreeRTOS tasks for data collection, storage, display,
 *                and sending to the cloud.
 *
 *                The function follows this flow:
 *                1. Establish Wi-Fi connection.
 *                2. Initialize the NTP protocol for time synchronization.
 *                3. Start MQTT client to connect to the cloud.
 *                4. Create a semaphore for synchronizing data storage operations.
 *                5. Create FreeRTOS tasks for:
 *                   - Data collection
 *                   - Data storage
 *                   - Data display
 *                   - Sending data to the cloud
 *
 * Author       : [Your Name or Company]
 *
 * Parameters   : None
 *
 * Return       : None
 ******************************************************************************/
static void operational_mode(void)
{
	read_and_print_device_info();

	/* Establish wifi connection */
	wifi_establish();

	/* Wait for sometime to connect wifi */
	vTaskDelay(5000 / portTICK_PERIOD_MS);

	/* Initialize drivers */
	init_ntp();

	/* Connect to cloud */
	mqtt_app_start();

	/* Wait for sometime to connect with the cloud */
	vTaskDelay(7000 / portTICK_PERIOD_MS);


	/* Create Semaphore for Storage Sync */
	dataSyncSemaphore = xSemaphoreCreateMutex();

	if (dataSyncSemaphore == NULL)
	{
		ESP_LOGE("QUEUE", "Failed to create storage semaphore!");
	}
	else
	{
		xSemaphoreGive(dataSyncSemaphore);
	}

	/* Create FreeRTOS tasks for different operations */
	if (xTaskCreate((void *)dataCollectionTask, "Data Collection Task", 4096, NULL, 4, NULL) != pdPASS) 
	{
		ESP_LOGE("TASK", "Failed to create Data Collection Task"); 
	}

	if (xTaskCreate(dataStorageTask, "Data Storage Task", 4096, NULL, 3, NULL) != pdPASS) 
	{
		ESP_LOGE("TASK", "Failed to create Data Storage Task");  
	}

	if (xTaskCreate(dataDisplayTask, "Data Display Task", 2048, NULL, 3, NULL) != pdPASS) 
	{
		ESP_LOGE("TASK", "Failed to create Data Collection Task"); 
	}
	if (xTaskCreate(dataSendCloudTask, "Data Send Cloud Task", 4096, NULL, 4, NULL) != pdPASS) 
	{
		ESP_LOGE("TASK", "Failed to create Data Send Task"); 
	}

}

static void wifi_establish(void)
{
	char ssid[WIFI_SSID_MAX_LEN] = {0}, password[WIFI_PASS_MAX_LEN] = {0};

	eeprom_read(EEPROM_WIFI_SSID_ADDR, (uint8_t *) ssid, WIFI_SSID_MAX_LEN);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	eeprom_read(EEPROM_WIFI_PASS_ADDR, (uint8_t *)password,WIFI_PASS_MAX_LEN);

	wifi_configured = false;

	/* Initialize Wi-Fi with stored credentials */
	if (strlen(ssid) > 0 && strlen(password) > 0)
	{
		wifi_init(ssid, password);
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	else
	{
		ESP_LOGW("WiFi", "Unable to Find Wi-Fi credentials found!");
	}
}


/*******************************************************************************
 * Function      : read_and_print_device_info
 *
 * Description   : Prints the device information stored in the BLE configuration
 *                 structure, including hardware version, software version,
 *                 device ID, and location. Displays the output in a formatted way.
 *
 * Parameters    : None
 *
 *******************************************************************************/

void read_and_print_device_info(void)
{

	printf("================================================================\n");
	printf("Welcome Fidility Solutions ESP32 IoT Smart AQI Hub System\n");

	/* Read from EEPROM into global structure */
	eeprom_read(EEPROM_LOCATION_ADDR, (uint8_t *)str_global_sensor_data.alocation, sizeof(str_global_sensor_data.alocation));
	eeprom_read(HW_VERSION_ADDR, (uint8_t *)str_global_sensor_data.ps8HVersion, sizeof(str_global_sensor_data.ps8HVersion));
	eeprom_read(SW_VERSION_ADDR, (uint8_t *)str_global_sensor_data.ps8SVersion, sizeof(str_global_sensor_data.ps8SVersion));
	eeprom_read(EEPROM_DEVICE_NAME_ADDR, (uint8_t *)str_global_sensor_data.as8DeviceId, sizeof(str_global_sensor_data.as8DeviceId));



	/* Print the information */
	printf("Hardware version: %s\n", str_global_sensor_data.ps8HVersion);
	printf("Software version: %s\n", str_global_sensor_data.ps8SVersion);
	printf("Device ID	: %s\n", str_global_sensor_data.as8DeviceId);
	printf("Location	: %s\n", str_global_sensor_data.alocation);
	printf("================================================================\n");
}


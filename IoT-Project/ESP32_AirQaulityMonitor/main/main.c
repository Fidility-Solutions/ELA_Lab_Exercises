#include<stdio.h>
#include "main.h"
#include "mqtts.h"
#include "data_display.h"


/* global sensor data */
sensor_data_t global_sensor_data;
sensor_data_t processed_data;

QueueHandle_t sensorDataQueue;
//QueueHandle_t commandDataQueue;
SemaphoreHandle_t dataSyncSemaphore = NULL;

EventGroupHandle_t xEventGroup;

//extern sensor_data_t global_sensor_data;

/* Static functions */
static void init_nvs(void);

static void factor_mode(void);

static void driver_init(void);

static void operational_mode(void);

static void eeprom_demo_test(void);

static void wifi_establish(void);

/* main function */
void app_main(void)
{
	printf("ESP32 Air Quality Monitor Starting...\n");
	int mode = 0;

	/* Initialize NVS */
	init_nvs();

	/* Create Event Group for multiple tasks to communicate based on shared event flags */
	xEventGroup = xEventGroupCreate();

	if(mode == FACTORY_MODE)
	{
		printf("Factory mode running...\n");
		factor_mode();
	}
	else
	{
		printf("Operational mode running...\n");
		operational_mode();
	}
}

/* Initialize the NVS */
static void init_nvs(void)
{
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		nvs_flash_erase();
		nvs_flash_init();
	}
	printf("NVS initialization sucessfull...\n");
}


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
			//mqtt_cloud_init();
			xEventGroupClearBits(xEventGroup, WIFI_CONNECTED_BIT);

		}
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}

}
static void driver_init(void)
{
	printf("Entered Driver initialization function\n");

	/* Initialze I2C driver & bme680 sensor configurations */
	BME680_task_start();
	if(!eeprom_check_presence())
	{
		ESP_LOGE("EEPROM", "EEPROM not detected. Please check connections");
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		esp_restart();
	}

	/* Initialize UART configuration for PM sensor */
	sds011_init();

	/* Initialize RTC for timestamp */
	init_RTC();

	/* ADC One-Shot Configuration */
	initialize_mq135_adc();

	lcd_init();

}
static void operational_mode(void)
{
	/* Initialize drivers */
	driver_init();

	/* Create the queue before starting tasks */
	sensorDataQueue = xQueueCreate(1, sizeof(global_sensor_data));

	/* Create Semaphore for Storage Sync */
	dataSyncSemaphore = xSemaphoreCreateMutex();

	if(sensorDataQueue == NULL)
	{
		ESP_LOGE("QUEUE","Failed to create  storage semaphore!\n");
	}
	if (dataSyncSemaphore == NULL)
	{
		ESP_LOGE("QUEUE", "Failed to create storage semaphore!");
	}
	else
	{
		xSemaphoreGive(dataSyncSemaphore);  // Ensure it starts available
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

	if (xTaskCreate(dataDisplayTask, "Data display Task", 2048, NULL, 3, NULL) != pdPASS) 
	{
		ESP_LOGE("TASK", "Failed to create Data Collection Task"); 
	}

}
static void eeprom_demo_test(void)
{
	ESP_LOGI("MAIN", "==== EEPROM DEMO ====");

	uint8_t config_data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

	uint16_t addr = 0x0100; /* Address to store data */

	ESP_LOGI("MAIN", "Writing to EEPROM at 0x%04X", addr);
	if (eeprom_write(addr, config_data, sizeof(config_data)) == ESP_OK)
	{
		ESP_LOGI("MAIN", "Write successful");
	}

	vTaskDelay(pdMS_TO_TICKS(10)); /* Ensure EEPROM has finished writing */

	/* Read data back */
	uint8_t read_data[16] = {0};
	ESP_LOGI("MAIN", "Reading from EEPROM at 0x%04X", addr);
	if (eeprom_read(addr, read_data, sizeof(read_data)) == ESP_OK)
	{
		ESP_LOGI("MAIN", "Read successful: ");
		for (int i = 0; i < sizeof(read_data); i++)
		{
			printf("0x%02X ", read_data[i]);
			if ((i + 1) % 8 == 0) printf("\n");
		}
		printf("\n");
	}
	else
	{
		ESP_LOGE("MAIN", "Read failed!");
	}
}
static void wifi_establish(void)
{
	char ssid[WIFI_SSID_MAX_LEN] = {0}, password[WIFI_PASS_MAX_LEN] = {0};
	if(wifi_configured)
	{
		eeprom_read(EEPROM_WIFI_SSID_ADDR, (uint8_t *) ssid, WIFI_SSID_MAX_LEN);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		eeprom_read(EEPROM_WIFI_PASS_ADDR, (uint8_t *)password,WIFI_PASS_MAX_LEN);

		printf("Read data SSID: %s, PASS: %s \n", ssid, password);

		wifi_configured = false;

		/* Initialize Wi-Fi with stored credentials */
		if (strlen(ssid) > 0 && strlen(password) > 0)
		{
			wifi_init(ssid, password);
		}
		else
		{
			ESP_LOGW("WiFi", "No stored Wi-Fi credentials found!");
		}
	}
}


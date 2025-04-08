#include "main.h"
#include "eeprom.h"
#include "data_display.h"


/* global sensor data */
STR_SENSOR_DATA global_sensor_data;
STR_SENSOR_DATA processed_data;

SemaphoreHandle_t dataSyncSemaphore = NULL;

EventGroupHandle_t xEventGroup;


/* Static functions */
static void init_nvs(void);

static void factor_mode(void);

static void operational_mode(void);


static void driver_init(void);

static void eeprom_rw_test(void);

static void wifi_establish(void);



static void uninit_i2c() 
{
    esp_err_t ret = i2c_driver_delete(I2C_NUM_0);
    if (ret == ESP_OK) 
    {
        printf("I2C uninitialized successfully!\n");
    } 
    else 
    {
        printf("Failed to uninitialize I2C: %s\n", esp_err_to_name(ret));
    }
}

/* main function */
void app_main(void)
{
	printf("ESP32 Air Quality Monitor Starting...\n");
	uint8_t mode = 0x00;
	uint8_t value = 0x01;
	uint8_t factory_mode = 0;

	/* Initialize NVS */
	init_nvs();

	/* Create Event Group for multiple tasks to communicate based on shared event flags */
	xEventGroup = xEventGroupCreate();

	/* Initialize I2C for EEPROM */
	init_RTC();

	driver_init();

	
	factory_mode = eeprom_read_byte(FACTORY_MODE_FLAG_ADDR);
	if(factory_mode != SELECT_FACTORY_MODE)
	{
		if (eeprom_write(FACTORY_MODE_FLAG_ADDR, &mode, 1) == ESP_OK)
		{
			factory_mode = 0;
		}
		factor_mode();
	}
	else
	{
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
}


static void factor_mode(void)
{
	/* Initialize BLE */
	ble_init();

	/* Manual test */
	/* char ssid[WIFI_SSID_MAX_LEN] = "FidilitySolutions", password[WIFI_PASS_MAX_LEN] = "fidility@123"; */
	while(1)
	{
		/* Manual test */
		/* wifi_init(ssid,password);
		 * vTaskDelay(5000 / portTICK_PERIOD_MS);
		 * mqtt_app_start();
		 */

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
			xEventGroupClearBits(xEventGroup, WIFI_CONNECTED_BIT);
			vTaskDelay(5000 / portTICK_PERIOD_MS);
			break;

		}
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}

}
static void driver_init(void)
{
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

	lcd_init();

	rgb_led_init();

}
static void operational_mode(void)
{
	/* Establish wifi connection */
	wifi_establish();
	
	/* Initialize drivers */
	init_ntp();
	
	/* Connect to cloud */
	 mqtt_app_start();


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
static void eeprom_rw_test(void)
{
	ESP_LOGI("MAIN", "==== EEPROM DEMO ====");

	uint8_t config_data[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

	uint16_t addr = 0x0100; /* Address to store data */

	ESP_LOGI("MAIN", "Writing to EEPROM at 0x%04X", addr);
	if(eeprom_write(addr, config_data, sizeof(config_data)) == ESP_OK)
	{
		ESP_LOGI("MAIN", "Write successful");
	}

	vTaskDelay(pdMS_TO_TICKS(10)); /* Ensure EEPROM has finished writing */

	/* Read data back */
	uint8_t read_data[16] = {0};
	if (eeprom_read(addr, read_data, sizeof(read_data)) == ESP_OK)
	{
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

	strcpy(ssid, "Rogers");
        strcpy(password, "rajapati");
	//if(wifi_configured)
	if(1)
	{
		//eeprom_read(EEPROM_WIFI_SSID_ADDR, (uint8_t *) ssid, WIFI_SSID_MAX_LEN);
	//	vTaskDelay(500 / portTICK_PERIOD_MS);
		//eeprom_read(EEPROM_WIFI_PASS_ADDR, (uint8_t *)password,WIFI_PASS_MAX_LEN);

		wifi_configured = false;

		/* Initialize Wi-Fi with stored credentials */
		if (strlen(ssid) > 0 && strlen(password) > 0)
		{
			wifi_init(ssid, password);
			vTaskDelay(5000 / portTICK_PERIOD_MS);
		/*	if (client != NULL)
			{
				ESP_LOGI("WiFi", "Stopping and destroying existing MQTT client.");
				esp_mqtt_client_stop(client);
				esp_mqtt_client_destroy(client);
				client = NULL; // Reset the client handle
			}*/

		//	ESP_LOGI("WiFi", "Starting MQTT client...");
		//	mqtt_app_start(); // Start MQTT client after Wi-Fi is reconnected

	//		mqtt_app_start();
			vTaskDelay(3000 / portTICK_PERIOD_MS);
		}
		else
		{
			ESP_LOGW("WiFi", "No stored Wi-Fi credentials found!");
		}
	}
}


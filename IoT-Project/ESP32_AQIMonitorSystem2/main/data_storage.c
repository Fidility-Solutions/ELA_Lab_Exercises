/* Header files */
#include "data_storage.h"
#include "spi.h"
#include "main.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"

//spi_device_handle_t spi_handle;

extern esp_mqtt_client_handle_t mqtt_client_handle;
STR_SENSOR_DATA str_processed_data;

/* Mutex for SPI synchronization */
//SemaphoreHandle_t spi_mutex;

static uint32_t u32Counter = COUNTER_INITIAL_VALUE;  		/* Current storage position */
static uint32_t u32CloudCounter = CLOUD_COUNTER_INITIAL_VALUE;  	/* Current cloud data upload position */

/*
 * Function     : dataStorageTask()
 *
 * Description  : This FreeRTOS task is responsible for handling sensor data storage and upload operations. It performs the following tasks:
 *                1. Initializes the SPI interface and creates a mutex to protect SPI operations.
 *                2. Monitors the Wi-Fi connection status to determine whether to store data locally or upload it to the cloud.
 *                3. If Wi-Fi is disconnected, the task stores sensor data locally in non-volatile memory (e.g., SPI flash), ensuring that only new data is stored.
 *                   It also handles sector erasure and data storage in 256-byte pages.
 *                4. If Wi-Fi is connected and there is stored data, the task uploads the data to the cloud by reading it from storage and publishing it via MQTT.
 *                5. It also handles wrap-around logic for the storage COUNTER_INITIAL_VALUEs to ensure that stored data does not overflow the available memory space.
 *                6. A semaphore is used to ensure synchronization when accessing shared sensor data, preventing race conditions.
 *
 * Parameters   : pvParameters - A pointer to parameters passed during task creation, not used here.
 *
 * Returns      : None
 *
 */
void dataStorageTask(void *pvParameters)
{ 
	esp_err_t eErrStat;

	STR_SENSOR_DATA last_sent_sensor_data = {0}; /* Track last sent data */
	STR_SENSOR_DATA str_local_sensor_copy;

	while (1)
	{
		/* Check if Wi-Fi is disconnected (store locally) */
		if ((gu8wificonnectedflag == 0) && (gu8initialconnectionflag == 1))
		{
			if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
			{
				memcpy(&str_local_sensor_copy, &str_global_sensor_data, sizeof(STR_SENSOR_DATA));

				/* Check if data is new before storing */
				if (memcmp(&last_sent_sensor_data, &str_local_sensor_copy, sizeof(STR_SENSOR_DATA)) != 0)
				{
					memcpy(&last_sent_sensor_data, &str_local_sensor_copy, sizeof(STR_SENSOR_DATA));

					/* Compute storage location */
					uint32_t u32SecNum = u32Counter / (16 * 256);
					uint32_t u32PageOffset = (u32Counter % (16 * 256)) / 256;

					/* Erase sector if needed */
					if (u32PageOffset == 0)
					{
						sector_erase(u32SecNum);
						vTaskDelay(pdMS_TO_TICKS(200));
					}

					/* Write to storage */
					write_data(u32Counter / 256, (uint8_t *)&str_local_sensor_copy, 
							sizeof(str_local_sensor_copy));

					/* Increment COUNTER_INITIAL_VALUE and handle wrap-around */
					u32Counter += 256; /* Increment by one page */
					if (u32Counter > MAX_ADDRESS)
					{
						/* Wrap around */
						u32Counter = 16;
					}
				}

				/* Release semaphore */
				xSemaphoreGive(dataSyncSemaphore);
			}
		}
		else if (u32CloudCounter < u32Counter) /* Wi-Fi connected, upload stored data */
		{
			read_data(u32CloudCounter / 256, (uint8_t *)&str_processed_data, 
					sizeof(str_processed_data));

			publish_sensor_data(&str_processed_data, mqtt_client_handle);

			u32CloudCounter += 256; /* Increment */

			if (u32CloudCounter >= u32Counter)
			{
				ESP_LOGI(SPI_TAG, "All data uploaded. Resetting COUNTER_INITIAL_VALUEs.");
				u32CloudCounter = 16;
				u32Counter = 16;
			}
		}

		vTaskDelay(pdMS_TO_TICKS(500));  /* General delay */
	}
}


#include "driver/spi_master.h"
#include "main.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <unistd.h>
#include "mqtt_client.h"
#include "data_collection.h"
#include "cJSON.h"



void command_receive_task(esp_mqtt_client_handle_t client)
{
	while (1)
        {
		if (xSemaphoreTake(sensor_data_mutex, portMAX_DELAY))
		{

			xSemaphoreGive(sensor_data_mutex);
                }
                vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}



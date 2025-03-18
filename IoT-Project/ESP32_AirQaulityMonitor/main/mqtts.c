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


static const char *TAG = "MQTT_EXAMPLE";

const char *aws_root_ca_pem = (const char *)aws_root_ca_pem_start;
const char *client_cert_pem = (const char *)client_cert_pem_start;
const char *client_key_pem = (const char *)client_key_pem_start;


//esp_mqtt_client_handle_t client = NULL;

char *sensor_data_to_json(sensor_data_t *data);


//void data_send_task(void *pvParameters)
void data_send_task(esp_mqtt_client_handle_t client)
{
	while (1) 
	{
		if (xQueuePeek(rawSensorQueue, &sensor_data, portMAX_DELAY) == pdTRUE)
		{
			if (xSemaphoreTake(sensor_data_mutex, portMAX_DELAY))
			{
				if (client != NULL) 
				{
					char *json_str = sensor_data_to_json(&sensor_data);
					if (json_str != NULL) {
						esp_mqtt_client_publish(client, "sensor/data", json_str, strlen(json_str), 1, 0);
						free(json_str);  // Free the allocated memory
					}
				}
				xSemaphoreGive(sensor_data_mutex);
			}
		}
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}

}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
	esp_mqtt_event_handle_t event = event_data;

	switch ((esp_mqtt_event_id_t) event_id)
       	{
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(TAG, "MQTT Connected!");
			//esp_mqtt_client_subscribe(client, "device/data", 1);

			break;

		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGE(TAG, "MQTT Disconnected! Retrying...");
			esp_mqtt_client_reconnect(client);
			break;

		case MQTT_EVENT_ERROR:
			ESP_LOGE(TAG, "MQTT Connection Error!");
			break;

		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(TAG, "MQTT Message Published!");
			break;

		case MQTT_EVENT_DATA:
			ESP_LOGI(TAG, "MQTT Data Received: Topic=%.*s Data=%.*s",
					event->topic_len, event->topic, event->data_len, event->data);

			// Pass the received data to the subscribe task for processing
                        // Assuming you have a queue to send commands to the processing task
			
			if (strncmp(event->topic, "commands/esp32", event->topic_len) == 0) {
				char *command = strndup(event->data, event->data_len);
				if (command != NULL) {
					xQueueSend(command_queue, &command, portMAX_DELAY);
					}
				}
			break;

		default:
			ESP_LOGW(TAG, "Unhandled MQTT Event ID: %d", event->event_id);
			break;
	}
}

esp_mqtt_client_handle_t mqtt_app_start(void)
{
	// Initialize the MQTT client configuration
	esp_mqtt_client_config_t mqtt_config = {
		.broker = {
			.address.uri = "mqtts://your-aws-endpoint.iot.us-east-1.amazonaws.com",
			.verification.certificate = aws_root_ca_pem,
		},
		.credentials = {
			.authentication = {
				.certificate = client_cert_pem,
				.key = client_key_pem,
			},
		},
	};

	esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_config);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
	esp_mqtt_client_start(client);

	return client;
}

char *sensor_data_to_json(sensor_data_t *data)
{
	cJSON *root = cJSON_CreateObject();  // Create JSON object

	// Add struct values to JSON
	cJSON_AddNumberToObject(root, "temperature", data->temp);
	cJSON_AddNumberToObject(root, "humidity", data->humidity);
	cJSON_AddNumberToObject(root, "PM 2.5", data->pm25);
	cJSON_AddNumberToObject(root, "Co2", data->co2);

	// Convert cJSON object to string
	char *json_string = cJSON_Print(root);

	// Free cJSON object (string memory needs to be freed by the caller)
	cJSON_Delete(root);

	return json_string;
}









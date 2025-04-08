/* Include Header files */
#include "mqtts.h"
#include "math.h"
static const char *TAG = "MQTT_EXAMPLE";


const char* client_key_pem = 
"-----BEGIN RSA PRIVATE KEY-----\n" \
		"MIIEowIBAAKCAQEAv+1kRKXOflr9+71IHwEHvEGwwVXLKgJagtMpjCyVax/dbWYm\n" \
		"LLJqB97DNXqeXHzPZojR/KplarARSsAmQVD8pqcIHb1MDPn2DxRMKXxqKKIk/aiL\n" \
		"squvc3EGm5rZDdzkl/iyoA/CVNckFtpBzHInlybmAbFwA8bry1JUIJWmwN6t+vjS\n" \
		"aGHSDLWHpgL3yRsvc++QbT5p+MwMZOLC1sYSsGabyXXAXxfIlDvZkwOg64xW7R5a\n" \
		"u0PP4BLvLgjrZRuyRnVnfAZiUU/+amizIrnNbbR9ek7TUh7tNMsaLcQ/rECySC9V\n" \
		"RC0fjDmOyl6P3UHEvhJTMaxzBb6tI7q7a2e7EQIDAQABAoIBAGIY2wv/V2jb5z85\n" \
		"Qtid9S8ywD5BNKNXKp39ghnkW8ayNcIJ2O81jd5R7KKz9qc9H+C/g9zJ1yZNsbjs\n" \
		"N2FhHJGthAd3Tpy/UhXpAPifX78/CPshn62F9RpjJcZ4wgJE6g4HDn2jMGeewxJv\n" \
		"Ifx2SGgIeL1azO8RSEnW3ar98opXIqrZdlYQNh+GT6FFoGrQDTkzniGkAoA9hP5q\n" \
		"QU22NCERoIBZSUH3K9BUcEhTkD8JGDb+klDDW1HLdPRv7fqlzPVqUjpp+G71uDY1\n" \
		"WNGgYhU9Vq30T5PAyunWCurdNC0K3N8EIfDz0ren8m/vS1nUc+Lfev5wA7Wdwria\n" \
		"bh193gECgYEA+LpNNqydrTTHY2yq+iQcwOyYyP981ieNQnh56Y8M479h7w+iVPW2\n" \
		"cPqco+Nx5w2nDByNSFc0qt06HjMQ1i7/GbfpWiDx8yQ7su4ROgsn0CGRRUN8FgUZ\n" \
		"SBkNR9h/skRT8tLNlbTIC8ULc/mpCHPg5VSMczLv075AfO5uXBpP6fECgYEAxYny\n" \
		"BlGtQsvd+frF1qY0l/y2nPUgUfHot6iQvp+bo4Z93V7UGk7J7l/pYgWIcZZaEzMi\n" \
		"IZyzNm1aq+pHXs/H2AiYY81FrQjp9j35je7tESnAODZABdXJ+Z0ynaWFPuCs9PEF\n" \
		"kp004uaX3kXBcvOnvGRSwWciCWXaVrVNDf5owyECgYBJm8DsFEuZjcwQNYGoA3Xq\n" \
		"9loE6e9E5XGdoU/a+fXDlMjj2cmdwRXb+57NtaroyRaNtAlvxjcU9eGUPGKYMFJY\n" \
		"hx/tB2DzJt7wBhpdBh4jLzvxtpYLYjjoKFnQ0FaCKEx7yT0EN/sX2JmaGeGXWcvN\n" \
		"sqbaFNH65QvqrRlF8peB0QKBgALBQ8DWzsXJthTQbIXr13vvKFTUDllWOFqnXS1r\n" \
		"aG5FZ+f4dJjRVHugls6pKRf8ctElu2ogl1dA+yy+nbQyzaSzqfxqVnhtUWX/w0U5\n" \
		"edDtdiLSjt4F90Ul7hjIwCz6US8xVVSQILaxZJjS/DmwMDyuYYEVASKEayEeci6p\n" \
		"x6hBAoGBAOZ62rZfyjZxTMSpk/ik/N4Y6GGLBMVNNefVfVkildqhE0PES57v/yT0\n" \
		"Rjw3mmwv6JFByBJdVmdBwyszeJX1ufC4T8FUqKXGyZveIMamolvIqxXll+s1xbm1\n" \
		"IISXUUGz8r8L1kQz0lY9mrzFesNfQYicjKD1vzdm4FS2Rxt7j8EY\n" \
		"-----END RSA PRIVATE KEY-----\n";
const char* client_cert_pem = 
"-----BEGIN CERTIFICATE-----\n" \
		"MIIDWjCCAkKgAwIBAgIVAM0jQsgNtHQlcK1jDWlGFvIyF4lFMA0GCSqGSIb3DQEB\n" \
		"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
		"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yNTAzMjYxMTE0\n" \
		"MzdaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
		"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC/7WREpc5+Wv37vUgf\n" \
		"AQe8QbDBVcsqAlqC0ymMLJVrH91tZiYssmoH3sM1ep5cfM9miNH8qmVqsBFKwCZB\n" \
		"UPympwgdvUwM+fYPFEwpfGoooiT9qIuyq69zcQabmtkN3OSX+LKgD8JU1yQW2kHM\n" \
		"cieXJuYBsXADxuvLUlQglabA3q36+NJoYdIMtYemAvfJGy9z75BtPmn4zAxk4sLW\n" \
		"xhKwZpvJdcBfF8iUO9mTA6DrjFbtHlq7Q8/gEu8uCOtlG7JGdWd8BmJRT/5qaLMi\n" \
		"uc1ttH16TtNSHu00yxotxD+sQLJIL1VELR+MOY7KXo/dQcS+ElMxrHMFvq0jurtr\n" \
		"Z7sRAgMBAAGjYDBeMB8GA1UdIwQYMBaAFFtheRC+4Ew9vkRvUe9jDnOlp+OKMB0G\n" \
		"A1UdDgQWBBTOxXe6h26dWClLBkeA5OKywvRIQjAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
		"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAg8wBsxl5D59anqDSAKubBx9d\n" \
		"S/Jn4yLzej9tjTIC4z9C+bWbRPLO2PwOUC//4Alg4oVWrYIpm/g17irp2RUOoLRd\n" \
		"xoieYB8+1Geg1cmjqMmfkFgI2lyAJdmvuWX7Sp55fjaMwGWdB13Me9uZdHNe6xf2\n" \
		"cV2HuQlsBzqcGIGboNtetdk5xgyFzCjmQrikk9vZGHtwqx4KDFgkVle+Lm9EKBx1\n" \
		"YRsUrQk9itatbKuGiQ2AGzHCVoefcs1RE9wkb6sGwmue9XRlSjHGazQKg0i7I2GG\n" \
		"lDK90f+3djUJho0rhp8oYgJKvCPK3CiqPkzWwKz/cJOym4835psmWeFcMTY0qA==\n" \
		"-----END CERTIFICATE-----\n";

const char* aws_root_ca_pem =
"-----BEGIN CERTIFICATE-----\n" \
		"MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
		"ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
		"b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
		"MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
		"b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
		"ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
		"9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
		"IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
		"VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
		"93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
		"jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
		"AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
		"A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
		"U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
		"N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
		"o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
		"5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
		"rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
		"-----END CERTIFICATE-----\n";


esp_mqtt_client_handle_t client = NULL;

static SemaphoreHandle_t spi_mutex;

volatile bool flash_operation_in_progress = false; // Flag for flash operations

char *json_str;

volatile bool is_data_updated = false;  // Flag to indicate new data

/*
 * This task checks if Wi-Fi is connected and if MQTT has started. If both conditions are true,
 * it attempts to send the latest sensor data to the cloud.
 *
 * 1. If Wi-Fi is connected and MQTT is started:
 *    - Lock shared data using a semaphore (xSemaphoreTake) to prevent concurrent access.
 *    - Copy the current global sensor data into a local variable (memcpy).
 *    - Compare the new data with the last sent data.
 *    - If the data has changed, convert the sensor data to JSON format (via publish_sensor_data).
 *    - Publish the data to the cloud (via MQTT).
 *    - Update the last sent data with the latest sensor data.
 *    - Release the semaphore (xSemaphoreGive) after publishing to allow other tasks access.
 *
 * 2. If Wi-Fi is not connected or MQTT is not started:
 *    - The task does nothing and will wait until Wi-Fi is available and MQTT is started.
 *
 * 3. Task Delay:
 *    - Wait for a brief period (500ms) before checking and processing again (vTaskDelay(500 / portTICK_PERIOD_MS)).
 */


void dataSendCloudTask(void *pvParameters)
{
    STR_SENSOR_DATA local_copy;  // Local copy for safe access
    static STR_SENSOR_DATA last_sent_data = {0};  // Store last sent data

    while (1)
    {
        if ((gs8wificonnectedflag == 1) && (gs8mqttstartedflag == 1))
        {
            if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))  // Lock before reading shared data
            {
                memcpy(&local_copy, &global_sensor_data, sizeof(STR_SENSOR_DATA));  // Copy shared data safely
                
                // Compare previous data and send only if updated
                if (memcmp(&last_sent_data, &local_copy, sizeof(STR_SENSOR_DATA)) != 0)
                {
                    publish_sensor_data(&local_copy, client);  // Send latest data
                    memcpy(&last_sent_data, &local_copy, sizeof(STR_SENSOR_DATA));  // Update last sent data
                }

                xSemaphoreGive(dataSyncSemaphore);  // Release lock AFTER publishing
            }
        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


void publish_sensor_data(STR_SENSOR_DATA *data, esp_mqtt_client_handle_t client)
{
	/* Create JSON object */
	cJSON *root = cJSON_CreateObject();

	/* Add struct values to JSON */
	cJSON_AddNumberToObject(root, "temperature", (double)((int)(data->bme680.f32Temperature * 100)) / 100.0);
	cJSON_AddNumberToObject(root, "humidity", (double)((int)(data->bme680.f32Humidity * 100)) / 100.0);
	cJSON_AddNumberToObject(root, "aqi", data->air_quality_index);
	cJSON_AddNumberToObject(root, "co2", data->bme680.u16GasRes);
	cJSON_AddNumberToObject(root, "pm25", data->sds011.u16PM25Val);
	cJSON_AddNumberToObject(root, "pm10", data->sds011.u16PM10Val);
	cJSON_AddStringToObject(root, "location", "Electronic City");

	/* Convert timestamp to epoch */
	struct tm time_info;
	time_t epoch_time = 0;

	if (strptime(data->atime_str, "%Y-%m-%d %H:%M:%S", &time_info))
	{
		epoch_time = mktime(&time_info);
	}
	cJSON_AddNumberToObject(root, "timestamp", epoch_time);

	/* Convert cJSON object to string */
	char *json_str = cJSON_Print(root);

	if (json_str != NULL)
	{
		printf("\nPublishing message: %s\n", json_str);

		if ((gs8wificonnectedflag == 1) && (gs8mqttstartedflag == 1)){

		/* Publish JSON to MQTT topic */
		esp_mqtt_client_publish(client, "sensor/data", json_str, strlen(json_str), 1, 0); }

		/* Free allocated memory for JSON string */
		free(json_str);
	}

	/* Free the cJSON object */
	cJSON_Delete(root);
}

/*static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_client_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t) event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected!");
            esp_mqtt_client_subscribe(client, "command/data", 1);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT Disconnected.");
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Connection Error!");
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT Message Published! Msg ID: %d", event->msg_id);
            // You can store msg_id and match it with publish calls for verification.
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Subscribed! Msg ID: %d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Unsubscribed! Msg ID: %d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT Data Received: Topic=%.*s Data=%.*s",
                     event->topic_len, event->topic, event->data_len, event->data);
            break;

        default:
            ESP_LOGW(TAG, "Unhandled MQTT Event ID: %d", event->event_id);
            break;
    }
    return ESP_OK;
}*/
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t) event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected!");
            esp_mqtt_client_subscribe(client, "command/data", 1);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT Disconnected.");
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT Connection Error!");
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT Message Published! Msg ID: %d", event->msg_id);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Subscribed to Topic! Msg ID: %d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT Unsubscribed from Topic! Msg ID: %d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT Data Received: Topic=%.*s Data=%.*s",
                     event->topic_len, event->topic, event->data_len, event->data);
            break;

        default:
            ESP_LOGW(TAG, "Unhandled MQTT Event ID: %d", event->event_id);
            break;
    }
}


/*static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) 
{
	esp_mqtt_event_handle_t event = event_data;

	switch ((esp_mqtt_event_id_t) event_id)
	{
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(TAG, "MQTT Connected!");
			esp_mqtt_client_subscribe(client, "command/data", 1);

			break;

		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGW(TAG, "MQTT Disconnected.");
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

			break;

		default:
			ESP_LOGW(TAG, "Unhandled MQTT Event ID: %d", event->event_id);
			break;
	}
}*/

void mqtt_app_start(void)
{

	/* Initialize the MQTT client configuration */
	esp_mqtt_client_config_t mqtt_config = {
		.broker = {
			.address.uri = "mqtts://a2uhwtdvqf6gg0-ats.iot.ap-south-1.amazonaws.com",
			.verification.certificate = aws_root_ca_pem,
		},
		.credentials = {
			.authentication = {
				.certificate = client_cert_pem,
				.key = client_key_pem,
			},
		},
		.network = {
                        .timeout_ms = 2000, // Set timeout for network operations to 2 seconds
                },
	};

	client = esp_mqtt_client_init(&mqtt_config);
	esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
	esp_mqtt_client_start(client);
	//mqtt_started_flag = 1;
}

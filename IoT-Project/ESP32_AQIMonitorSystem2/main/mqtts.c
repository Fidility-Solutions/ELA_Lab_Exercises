/******************************************************************************
 * File         : mqtts.c
 *
 * Description  : This file implements MQTT client functionality for secure 
 *                communication with an MQTT broker using the MQTT over TLS 
 *                (MQTTS) protocol. The client interacts with an AWS IoT broker 
 *                or any other MQTT broker supporting secure connections. It 
 *                handles connecting, subscribing, publishing sensor data, and 
 *                handling various MQTT events.
 *
 * Author       : Fidility Solutions
 *
 * Reference    : ESP-IDF MQTT Client Library Documentation, AWS IoT MQTT 
 *                Documentation.
 *******************************************************************************/


/* Include Header files */
#include "mqtts.h"
#include "eeprom.h"
#include <time.h>
#include "main.h"
#include "cJSON.h"
#include "esp_timer.h"

uint8_t u8CloudConnect = 1;
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

static void control_command(const char *ps8Data, uint32_t u32DataLen);
esp_mqtt_client_handle_t mqtt_client_handle = NULL;

static SemaphoreHandle_t spi_mutex;

volatile uint32_t u32LastPublishedTime = 0;  		/* Tracks the last publish time */
volatile uint32_t u32SetPublishInterval = 30000;  	/* 10 seconds (in milliseconds) */

unsigned long millis() 
{
	return esp_timer_get_time() / 1000;  		/* Convert microseconds to milliseconds */
}
/*
 * Function     : dataSendCloudTask()
 *
 * Description  : This FreeRTOS task is responsible for sending sensor data to the cloud via MQTT. 
 *                It checks if the Wi-Fi connection and MQTT connection are established, and if so, 
 *                it locks the shared data, compares the newly read data with the last sent data to 
 *                avoid sending duplicate data, and publishes the data to the cloud.
 *
 * Parameters   : pvParameters - A pointer to parameters passed during task creation, not used here.
 *
 * Returns      : None
 *
 * Notes        : The task runs in an infinite loop, periodically checking for data updates and sending
 *                them to the cloud when necessary. It utilizes a semaphore to ensure safe access to 
 *                shared sensor data and avoid race conditions.
 */


static void control_command(const char *ps8Data, uint32_t u32DataLen)
{
        char ps8JsonData[u32DataLen];
        if (!ps8JsonData)
        {
                ESP_LOGE(MQTTS_TAG, "Failed to allocate memory for JSON parsing");
                return;
        }

        memcpy(ps8JsonData, ps8Data, u32DataLen);       /* Copy data into the allocated memory */
        ps8JsonData[u32DataLen] = '\0';                 /* Ensure null-termination */

        cJSON *root = cJSON_Parse(ps8JsonData);
        if (!root)
        {
                ESP_LOGE(MQTTS_TAG, "Failed to parse JSON");
                free(ps8JsonData);
                return;
        }

        /* Parse and handle the 'action' field */
        cJSON *action = cJSON_GetObjectItem(root, "action");
        if (cJSON_IsString(action))
        {
                if (strcmp(action->valuestring, "setInterval") == 0)
                {
                        cJSON *interval = cJSON_GetObjectItem(root, "interval");
                        if (cJSON_IsNumber(interval))
                        {

                                /* Map intervals to seconds */
                                switch ((int)interval->valuedouble)
                                {
                                        case 1:
                                                u32SetPublishInterval = 1;
                                                ESP_LOGI(MQTTS_TAG, "Interval set to 1 second");
                                                break;
                                        case 2:
                                                u32SetPublishInterval = 2;
                                                ESP_LOGI(MQTTS_TAG, "Interval set to 2 seconds");
                                                break;
                                        case 5:
                                                u32SetPublishInterval = 5;
                                                ESP_LOGI(MQTTS_TAG, "Interval set to 5 seconds");
                                                break;
                                        case 60:
                                                u32SetPublishInterval = 60;
                                                ESP_LOGI(MQTTS_TAG, "Interval set to 1 minute");
                                                break;
                                        case 120:
                                                u32SetPublishInterval = 120;
                                                ESP_LOGI(MQTTS_TAG, "Interval set to 2 minutes");
                                                break;
                                        case 300:
                                                u32SetPublishInterval = 300;
                                                ESP_LOGI(MQTTS_TAG, "Interval set to 5 minutes");
                                                break;
                                        default:
                                                ESP_LOGW(MQTTS_TAG, "Unsupported interval: %d", (int)interval->valuedouble);
                                                break;
                                }

                        }
                        else
                        {
                                ESP_LOGW(MQTTS_TAG, "Invalid or missing 'interval' field");
                        }
                }
                else if (strcmp(action->valuestring, "eraseEEPROM") == 0)
                {
                        ESP_LOGI(MQTTS_TAG, "EEPROM erase command received");
                        erase_eeprom_chip();
                }
                else
                {
                        ESP_LOGW(MQTTS_TAG, "Unsupported action: %s", action->valuestring);
                }
        }
        else
        {
                ESP_LOGW(MQTTS_TAG, "Missing or invalid 'action' field");
        }

        cJSON_Delete(root);  /* Free cJSON object */
        free(ps8JsonData);     /* Free allocated memory */
}


void dataSendCloudTask(void *pvParameters)
{
	char *ps8JsonStr;
	STR_SENSOR_DATA str_sensor_local_copy;  		/* Local copy for safe access */
	static STR_SENSOR_DATA last_sent_sensor_data = {0};  	/* Store last sent data */
	uint32_t u32CurrentTime = millis();


	while (1)
	{
		/* Get the current time (in milliseconds) */
		unsigned long u32CurrentTime = millis();  /* Or use another method to get the current time in ms */
		/* If the desired interval has passed, send the data */
		//if (u32CurrentTime - u32LastPublishedTime >= u32SetPublishInterval)
		//{
			if ((gu8wificonnectedflag == 1) && (gu8mqttstartedflag == 1))
			{
				/* Lock before reading shared data */
				if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
				{
					/* Copy shared data safely */
					memcpy(&str_sensor_local_copy, &str_global_sensor_data, sizeof(STR_SENSOR_DATA));

					/* Compare previous data and send only if updated */
					if (memcmp(&last_sent_sensor_data, &str_sensor_local_copy, sizeof(STR_SENSOR_DATA)) != 0)
					{
						/* Send latest data */
						publish_sensor_data(&str_sensor_local_copy, mqtt_client_handle);

						/* Update last sent data */
						memcpy(&last_sent_sensor_data, &str_sensor_local_copy, sizeof(STR_SENSOR_DATA));
					}

					/* Release lock AFTER publishing */
					xSemaphoreGive(dataSyncSemaphore);
				}
			}
			/* Update the last publish time */
			//u32LastPublishedTime = u32CurrentTime;
		//}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

/*
 * Function     : publish_sensor_data()
 *
 * Description  : This function constructs a JSON object containing sensor data and publishes it to an MQTT broker.
 *                The function uses the cJSON library to create the JSON object, adding sensor data such as 
 *                temperature, humidity, air quality index (AQI), CO2 concentration, PM2.5, PM10, location, and timestamp.
 *                The timestamp is converted from a string format to epoch time before being included in the JSON object.
 *                If the Wi-Fi and MQTT connections are active, the JSON object is published to the "sensor/data" MQTT topic.
 *                The function also manages memory by freeing allocated memory after the JSON string is used.
 *
 * Parameters   : pstrData - A pointer to the sensor data structure containing the sensor readings.
 *                client - The MQTT client handle used for publishing the message.
 *
 * Returns      : None
 *
 * Notes        : The function utilizes the cJSON library to format the sensor data into a JSON object and send it
 *                to the cloud via MQTT. It ensures proper memory management by freeing the allocated JSON string 
 *                after publishing. The function also ensures that the Wi-Fi and MQTT connections are active 
 *                before attempting to publish the data.
 */
void publish_sensor_data(STR_SENSOR_DATA *pstrData, esp_mqtt_client_handle_t client)
{
	/* Create JSON object */
	cJSON *root = cJSON_CreateObject();

	/* Add struct values to JSON */
	cJSON_AddNumberToObject(root, "temperature", (double)((int)(pstrData->bme680.f32Temperature * 100)) / 100.0);
	cJSON_AddNumberToObject(root, "humidity", (double)((int)(pstrData->bme680.f32Humidity * 100)) / 100.0);
	cJSON_AddNumberToObject(root, "aqi", pstrData->air_quality_index);
	cJSON_AddNumberToObject(root, "co2", pstrData->bme680.u16GasRes);
	cJSON_AddNumberToObject(root, "pm25", pstrData->sds011.u16PM25Val);
	cJSON_AddNumberToObject(root, "pm10", pstrData->sds011.u16PM10Val);
	cJSON_AddStringToObject(root, "location", "Electronic City");

	/* Convert timestamp to epoch */
	struct tm time_info;
	time_t epoch_time = 0;

	if (strptime(pstrData->atime_str, "%Y-%m-%d %H:%M:%S", &time_info))
	{
		epoch_time = mktime(&time_info);
	}
	cJSON_AddNumberToObject(root, "timestamp", epoch_time);

	/* Convert cJSON object to string */
	char *ps8JsonStr = cJSON_Print(root);

	if (ps8JsonStr != NULL)
	{
		printf("\nPublishing message: %s\n", ps8JsonStr);

		if ((gu8wificonnectedflag == 1) && (gu8mqttstartedflag == 1)){

			/* Publish JSON to MQTT topic */
			esp_mqtt_client_publish(client, "sensor/data", ps8JsonStr, strlen(ps8JsonStr), 1, 0); }

		/* Free allocated memory for JSON string */
		free(ps8JsonStr);
	}

	/* Free the cJSON object */
	cJSON_Delete(root);
}

/*
 * Function     : mqtt_event_handler()
 *
 * Description  : This function handles various MQTT events such as connection status, message publishing,
 *                subscription, and data reception. It processes the events received from the MQTT client and
 *                takes appropriate actions based on the event type.
 *
 *                - On successful connection (`MQTT_EVENT_CONNECTED`), it subscribes to a specific topic (`command/data`).
 *                - On disconnection (`MQTT_EVENT_DISCONNECTED`), it logs a warning.
 *                - On error (`MQTT_EVENT_ERROR`), it logs an error message.
 *                - On successful message publication (`MQTT_EVENT_PUBLISHED`), it logs the message ID.
 *                - On successful subscription (`MQTT_EVENT_SUBSCRIBED`), it logs the message ID.
 *                - On successful unsubscription (`MQTT_EVENT_UNSUBSCRIBED`), it logs the message ID.
 *                - On receiving data (`MQTT_EVENT_DATA`), it logs the topic and data.
 *
 * Parameters   :
 *      - handler_args : Arguments passed to the event handler (unused here).
 *      - base         : Event base (unused here).
 *      - event_id     : Event ID which determines the type of MQTT event.
 *      - event_data   : Data associated with the event (contains the event details such as message, topic, etc.).
 *
 * Returns      : None
 *
 * Notes        : This event handler is called whenever an MQTT event occurs, and it helps manage the
 *                interaction with the MQTT broker, handling connection, data exchange, and other events.
 *                The function uses `ESP_LOGI` and `ESP_LOGW` to log information and warnings, helping to track
 *                the MQTT client's activity.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
	char u8Location[32];
	eeprom_read(EEPROM_LOCATION_ADDR, (uint8_t *) &u8Location, sizeof(u8Location));

	/* Dynamically create the topic string */
	char s8Topic[64];
	sprintf(s8Topic, "esp32/%s/control", u8Location);

	esp_mqtt_event_handle_t event = event_data;
	esp_mqtt_client_handle_t client = event->client;

	switch ((esp_mqtt_event_id_t) event_id)
	{
		case MQTT_EVENT_CONNECTED:
			ESP_LOGI(MQTTS_TAG, "MQTT Connected!");
			esp_mqtt_client_subscribe(client, s8Topic, 1);
			u8CloudConnect = 1;
			break;

		case MQTT_EVENT_DISCONNECTED:
			ESP_LOGW(MQTTS_TAG, "MQTT Disconnected.");
			break;

		case MQTT_EVENT_ERROR:
			ESP_LOGE(MQTTS_TAG, "MQTT Connection Error!");
			break;

		case MQTT_EVENT_PUBLISHED:
			ESP_LOGI(MQTTS_TAG, "MQTT Message Published! Msg ID: %d", event->msg_id);
			break;

		case MQTT_EVENT_SUBSCRIBED:
			ESP_LOGI(MQTTS_TAG, "MQTT Subscribed to Topic! Msg ID: %d", event->msg_id);
			break;

		case MQTT_EVENT_UNSUBSCRIBED:
			ESP_LOGI(MQTTS_TAG, "MQTT Unsubscribed from Topic! Msg ID: %d", event->msg_id);
			break;

		case MQTT_EVENT_DATA:
			ESP_LOGI(MQTTS_TAG, "MQTT Data Received: Topic=%.*s Data=%.*s",
					event->topic_len, event->topic, event->data_len, event->data);
			control_command(event->data, event->data_len);
			break;

		default:
			ESP_LOGW(MQTTS_TAG, "Unhandled MQTT Event ID: %d", event->event_id);
			break;
	}
}

/*
 * Function     : mqtt_app_start()
 *
 * Description  : This function initializes and starts the MQTT client with a given configuration.
 *                The function sets up the MQTT broker details, including the URI, root CA certificate for SSL/TLS
 *                verification, client certificate, and key for client authentication.
 *                It also configures network timeouts and registers an event handler for MQTT events.
 *                After the configuration, the MQTT client is started to begin communication with the broker.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Notes        : The MQTT client is configured to communicate securely (MQTT over TLS) with the AWS IoT broker.
 *                The client will authenticate using the provided certificate and key.
 *                The function registers an event handler to handle incoming MQTT events (like message delivery, connection status, etc.).
 *                The `mqtt_started_flag` has been commented out, but it could be used to track the state of the MQTT client.
 */
void mqtt_app_start(void)
{

	/* Initialize the MQTT client configuration */
	esp_mqtt_client_config_t mqtt_config = 
	{
		.broker = 
		{
			.address.uri = "mqtts://a2uhwtdvqf6gg0-ats.iot.ap-south-1.amazonaws.com",
			.verification.certificate = aws_root_ca_pem,
		},
		.credentials = 
		{
			.authentication = 
			{
				.certificate = client_cert_pem,
				.key = client_key_pem,
			},
		},
		.network = 
		{
			.timeout_ms = 2000, /* Set timeout for network operations to 2 seconds */
		},
	};

	mqtt_client_handle = esp_mqtt_client_init(&mqtt_config);
	esp_mqtt_client_register_event(mqtt_client_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
	esp_mqtt_client_start(mqtt_client_handle);
	//mqtt_started_flag = 1;
}


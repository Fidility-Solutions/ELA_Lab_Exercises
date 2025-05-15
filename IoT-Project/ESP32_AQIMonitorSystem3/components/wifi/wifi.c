
/*******************************************************************************
 * File:        wifi_manager.c
 *
 * Description: This file implements Wi-Fi connectivity and MQTT integration for ESP32.
 *              It initializes Wi-Fi in station mode, handles various Wi-Fi and IP events,
 *              manages retry logic on disconnection, and controls the MQTT client state 
 *              based on Wi-Fi connectivity. If initial connection fails after multiple 
 *              retries, the system triggers a watchdog reset to ensure recovery.
 *
 *              Key Functionalities:
 *              - Wi-Fi STA initialization with SSID/password.
 *              - Retry mechanism with a maximum limit.
 *              - Watchdog reset on persistent connection failure.
 *              - MQTT start on successful IP acquisition.
 *              - MQTT stop on disconnection.
 *              - Event group synchronization using FreeRTOS.
 *
 * Author:      FidilitySolutions
 *
 * Reference:   ESP-IDF Programming Guide 
 *              https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html
 *
 *******************************************************************************/

#include "wifi.h"
#include "esp_task_wdt.h"
#include "mqtts.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "event_manager.h"

/* Static functions */
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;


/* Global Variables */
/* Flag to check if the Wi-Fi is connected (1 = Connected, 0 = Not connected) */
volatile uint8_t gu8wificonnectedflag = 0;

/* Flag to check if the MQTTS protocol is started (1 = Connected, 0 = Not connected) */
volatile uint8_t gu8mqttstartedflag = 0;

/* Flag to check if the device is connecting to Wi-Fi for the first time (1 = First-time connection, 0 = Reconnection) */
volatile uint8_t gu8initialconnectionflag = 0;

extern esp_mqtt_client_handle_t mqtt_client_handle;


/*
 * Function     : wifi_event_handler()
 *
 * Description  : Handles Wi-Fi and IP events during the connection lifecycle.
 *                - Initiates connection on start
 *                - Manages retry attempts on disconnection
 *                - Handles successful IP acquisition
 *                - Triggers watchdog reset if retries exceed threshold
 *                - Manages MQTT client based on Wi-Fi state
 *
 * Parameters   :
 *      pArg         - Pointer to user-defined data (not used here)
 *      event_base   - Event base identifier (WIFI_EVENT or IP_EVENT)
 *      u32EventId   - Specific event ID (e.g., WIFI_EVENT_STA_DISCONNECTED)
 *      pEventData   - Event-specific data (e.g., IP info on successful IP)
 */

void wifi_event_handler(void *pArg, esp_event_base_t event_base, int32_t u32EventId, void *pEventData) 
{
	static uint8_t u8RetryCount = 0;

	/* Handle Wi-Fi related events */
	if (event_base == WIFI_EVENT) 
	{
		if (u32EventId == WIFI_EVENT_STA_START) 
		{
			/* Station started, initiate connection */
			ESP_LOGI(WIFI_TAG, "WiFi station started, connecting to AP...");
			esp_wifi_connect();
		}
		else if (u32EventId == WIFI_EVENT_STA_CONNECTED) 
		{
			/* Successfully connected to the Access Point */
			ESP_LOGI(WIFI_TAG, "Connected to WiFi AP.");
		}
		else if (u32EventId == WIFI_EVENT_STA_DISCONNECTED) 
		{
			/* Wi-Fi got disconnected */
			ESP_LOGW(WIFI_TAG, "WiFi disconnected.");
			gu8wificonnectedflag = 0;

			/* Handle case where initial connection hasn't yet succeeded */
			if (gu8initialconnectionflag == 0) 
			{
				if (u8RetryCount < MAX_RETRY) 
				{
					/* Retry connection */
					ESP_LOGI(WIFI_TAG, "Retrying WiFi connection... (%d/%d)", 
							u8RetryCount + 1, MAX_RETRY);

					esp_wifi_connect();
					u8RetryCount++;
				}
				else 
				{
					/* All retries exhausted – signal failure and trigger watchdog reset */
					ESP_LOGW(WIFI_TAG, "WiFi connection failed after %d attempts.", MAX_RETRY);

					xEventGroupClearBits(xEventGroup, WIFI_CONNECTED_BIT);
					xEventGroupSetBits(xEventGroup, WIFI_FAIL_BIT);

					esp_restart();

				}
			} 
			else 
			{
				/* Already connected earlier, handle post-connection disconnection */
				ESP_LOGW(WIFI_TAG, "WiFi disconnected after initial connection.");
				if (gu8mqttstartedflag == 1) 
				{
					ESP_LOGI(WIFI_TAG, "Stopping and destroying MQTT client due to Wi-Fi disconnection...");
					esp_mqtt_client_stop(mqtt_client_handle);   /* Stop MQTT client */
					gu8mqttstartedflag = 0;
				}
				esp_wifi_connect(); /* Attempt reconnection */
			}
		}
	}
	/* Handle IP events (e.g., got IP) */
	else if (event_base == IP_EVENT && u32EventId == IP_EVENT_STA_GOT_IP)
	{
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)pEventData;
		ESP_LOGI(WIFI_TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
		
		/* Call MQTTS to connect to cloud */
		//mqtt_app_start();
		//vTaskDelay(7000 / portTICK_PERIOD_MS);

		/* Reset retry count and update flags */
		u8RetryCount = 0;
		gu8initialconnectionflag = 1;
		gu8wificonnectedflag = 1;

		xEventGroupSetBits(xEventGroup, WIFI_CONNECTED_BIT);
		xEventGroupClearBits(xEventGroup, WIFI_FAIL_BIT);

		/* Start MQTT client if not started */
		if(gu8mqttstartedflag == 0)
		{
			ESP_LOGI(WIFI_TAG, "Starting MQTT client after Wi-Fi connection...");
			esp_mqtt_client_start(mqtt_client_handle);
			gu8mqttstartedflag = 1;
		}
	}
}

/*
 * Function     : wifi_init()
 *
 * Description  : Initializes the Wi-Fi in station mode (STA) to connect to a given SSID with password.
 *                This function sets up the network interface, event loop, Wi-Fi config, and event handlers.
 *                It disables power saving mode for a more stable connection.
 *
 * Parameters   :
 *      s8SSID     - Pointer to a string containing the Wi-Fi SSID.
 *      s8Password - Pointer to a string containing the Wi-Fi password.
 *
 * Returns      : None. Errors are handled via ESP_ERROR_CHECK().
 */
void wifi_init(const char *ps8SSID, const char *ps8Password)
{
	ESP_LOGI(WIFI_TAG, "Wi-Fi initializing...");

	/* Initialize the underlying TCP/IP stack */
	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	/* Register event handlers */
	ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
				&wifi_event_handler, NULL, &instance_any_id));

	ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
				&wifi_event_handler, NULL, &instance_got_ip));

	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	/* Disable power saving for stable connection */
	esp_wifi_set_ps(WIFI_PS_NONE);
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	wifi_config_t wifi_config = {};

	strncpy((char *)wifi_config.sta.ssid, ps8SSID, WIFI_SSID_MAX_LEN);
	strncpy((char *)wifi_config.sta.password, ps8Password, WIFI_PASS_MAX_LEN);

	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(WIFI_TAG, "Wi-Fi Init Completed. Connecting to: %s", ps8SSID);
}

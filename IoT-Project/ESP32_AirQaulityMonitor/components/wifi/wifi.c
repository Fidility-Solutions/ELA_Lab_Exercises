#include "wifi.h"


/* Static functions */
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;


/**Wi-Fi Event Handler**/
/* reference : https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/provisioning/wifi_provisioning.html */


void wifi_event_handler(void *arg, esp_event_base_t event_base,
		int32_t event_id, void *event_data) 
{
	static int retry_count = 0;

	if (event_base == WIFI_EVENT) 
	{
		if (event_id == WIFI_EVENT_STA_START) 
		{
			ESP_LOGI(WIFI_TAG, "WiFi station started, connecting to AP");
			esp_wifi_connect();
		} 
		else if (event_id == WIFI_EVENT_STA_CONNECTED) 
		{
			ESP_LOGI(WIFI_TAG, "Connected to WiFi AP");
		} 
		else if (event_id == WIFI_EVENT_STA_DISCONNECTED) 
		{
			if (retry_count < MAX_RETRY) 
			{
				ESP_LOGI(WIFI_TAG, "WiFi disconnected, retrying... (%d/%d)", retry_count + 1, MAX_RETRY);
				esp_wifi_connect();
				retry_count++;
			} 
			else 
			{
				ESP_LOGW(WIFI_TAG, "WiFi connection failed after %d attempts", MAX_RETRY);
				xEventGroupClearBits(xEventGroup, WIFI_CONNECTED_BIT);
				xEventGroupSetBits(xEventGroup, WIFI_FAIL_BIT);
				retry_count = 0;

				/* Wait before trying again */
				vTaskDelay(10000 / portTICK_PERIOD_MS);
				esp_wifi_connect();
			}
		}
	} 
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
	{
		ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
		ESP_LOGI(WIFI_TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
		
		retry_count = 0;
		
		xEventGroupSetBits(xEventGroup, WIFI_CONNECTED_BIT);
		xEventGroupClearBits(xEventGroup, WIFI_FAIL_BIT);

		/* If we have unsynchronized data, set the sync bit */
		//if (flash_metadata.data_count > 0) 
		//{
		//	xEventGroupSetBits(xEventGroup, NEED_SYNC_BIT);
		//}
	}
}

void wifi_init(const char *ssid, const char *password) 
{
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

	strncpy((char *)wifi_config.sta.ssid, ssid, WIFI_SSID_MAX_LEN);
	strncpy((char *)wifi_config.sta.password, password, WIFI_PASS_MAX_LEN);
	wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI("WiFi", "Wi-Fi Init Completed. Connecting to: %s", ssid);
}



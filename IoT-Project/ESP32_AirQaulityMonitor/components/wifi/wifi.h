#ifndef WIFI_H
#define WIFI_H

/* Header files */
#include <stddef.h>    			
#include <string.h>    			
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"  	
#include  <esp_event.h>              	
#include "esp_wifi.h"               	
#include "esp_netif.h"              	
#include "eeprom.h"
#include "ble.h"
#include "wifi_provisioning/scheme_ble.h"
#include "event_manager.h"

#define WIFI_SSID_MAX_LEN 32
#define WIFI_PASS_MAX_LEN 64
#define MAX_RETRY	5
#define WIFI_TAG	"WIFI"


void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);
void wifi_init(const char *ssid, const char *password);

#endif /* WIFI_H */

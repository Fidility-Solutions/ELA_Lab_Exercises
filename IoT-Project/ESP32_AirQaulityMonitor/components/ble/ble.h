#ifndef BLE_H
#define BLE_H

/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "sdkconfig.h"
#include "wifi.h"
#include "eeprom.h"
#include "event_manager.h"

/* Define Macros */
#define CONFIG_SERVICE_UUID    	0x00FF
#define WIFI_CHAR_UUID         	0xFF01
#define LOCATION_CHAR_UUID     	0xFF02
#define HW_VERSION_CHAR_UUID   	0xFF03
#define SW_VERSION_CHAR_UUID   	0xFF04

#define DEVICE_NAME            	"ESP32_CONFIG"
#define CONFIG_APP_ID          	0
#define MAX_DATA_LENGTH        	512

#define BLE_TAG 		"BLE"
#define MAX_CRED_SIZE      	32


/* Global Variables */
extern bool wifi_configured;

/* Function prototypes */
void ble_init(void);
#endif /* BLE_H */

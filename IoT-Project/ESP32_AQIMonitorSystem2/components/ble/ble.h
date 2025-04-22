#ifndef BLE_H
#define BLE_H

/* Header files */
#pragma once
//#include "event_manager.h"
#include <stdbool.h>
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

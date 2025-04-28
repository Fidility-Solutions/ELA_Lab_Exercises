#ifndef BLE_H
#define BLE_H

/* Header files */
#pragma once
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
#define EEPROM_SIZE 		2048  /* Define the total EEPROM size */

/* Global Variables */
extern bool wifi_configured;

/* Function prototypes */
void ble_init(void);

typedef struct 
{
    	char as8blelocation[32];                  /*     Location string         */
    	char as8bleDeviceId[32];                /*     Device ID               */
    	char ps8bleHVersion[16];                /*     Hardware version        */
    	char ps8bleSVersion[16];                /*     Software version        */
}STR_BLE_PARAM_CONFIG;

extern STR_BLE_PARAM_CONFIG str_ble_param_config;


#endif /* BLE_H */

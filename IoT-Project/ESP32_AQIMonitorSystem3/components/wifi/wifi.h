#ifndef WIFI_H
#define WIFI_H

#pragma once

/* Header files */
#include  <esp_event.h>              	
			

/* Define Macros */
#define WIFI_SSID_MAX_LEN 	32
#define WIFI_PASS_MAX_LEN 	64
#define MAX_RETRY		5
#define WIFI_TAG		"WIFI"
#define WATCHDOG_TIMEOUT 	5



extern volatile uint8_t gu8wificonnectedflag;
extern volatile uint8_t gu8mqttstartedflag;
extern volatile uint8_t gu8initialconnectionflag;

/* Function prototypes */
void wifi_event_handler(void *PArg, esp_event_base_t event_base,
                        int32_t s32EventId, void *pEventData);
void wifi_init(const char *ps8SSID, const char *s8Password);

#endif /* WIFI_H */

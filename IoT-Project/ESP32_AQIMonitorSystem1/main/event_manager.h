/* event_manager.h */
#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/* Declare Event Group (Extern) */
extern EventGroupHandle_t xEventGroup;

/* Event group bits */
#define WIFI_CONNECTED_BIT      BIT1	/* Wi-Fi is connected */
#define WIFI_FAIL_BIT           BIT2
#define MQTT_CONNECTED_BIT      BIT3	/* MQTT is connected */
#define NEED_SYNC_BIT           BIT4
#define BLE_WIFI_CONNECT_BIT	BIT0


#endif /* EVENT_MANAGER_H */


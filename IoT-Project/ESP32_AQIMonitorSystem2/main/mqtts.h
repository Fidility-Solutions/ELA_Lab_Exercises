#ifndef MQTTS_H
#define MQTTS_H

#pragma once
#include "mqtt_client.h"

#define MQTTS_TAG	"MQTT"


extern uint8_t u8CloudConnect;

/* Declare the mqtt config (but don't initialize it here) */
extern esp_mqtt_client_config_t mqtt_cfg;

/* Function Prototypes */
void mqtt_app_start(void);
void dataSendCloudTask(void *pvParameters);


extern uint32_t u32LastPublishedTime;              /* Tracks the last publish time */

extern uint32_t u32SetPublishInterval;         /* 1 min (in milliseconds) */

extern uint32_t u32CurrentTime;


uint32_t millis();

#endif /* MQTTS_H */

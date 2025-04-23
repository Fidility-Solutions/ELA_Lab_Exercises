#ifndef MQTTS_H
#define MQTTS_H

#pragma once
#include "mqtt_client.h"

#define MQTTS_TAG	"MQTT"


extern volatile uint32_t u32SetPublishInterval;
extern uint8_t u8CloudConnect;
/* Declare the mqtt config (but don't initialize it here) */
extern esp_mqtt_client_config_t mqtt_cfg;

/* Function Prototypes */
void mqtt_app_start(void);
void dataSendCloudTask(void *pvParameters);

#endif /* MQTTS_H */

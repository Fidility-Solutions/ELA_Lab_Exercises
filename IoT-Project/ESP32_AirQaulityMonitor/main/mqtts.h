#ifndef MQTTS_H
#define MQTTS_H

#include "mqtt_client.h"

// First, declare the extern symbols that the linker will resolve
extern const uint8_t aws_root_ca_pem_start[] asm("_binary_aws_root_ca_pem_start");
extern const uint8_t aws_root_ca_pem_end[] asm("_binary_aws_root_ca_pem_end");
extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_pem_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_pem_end");
extern const uint8_t client_key_pem_start[] asm("_binary_client_key_pem_start");
extern const uint8_t client_key_pem_end[] asm("_binary_client_key_pem_end");

// Declare the global pointers (but don't initialize them in the header)
extern const char *aws_root_ca_pem;
extern const char *client_cert_pem;
extern const char *client_key_pem;

// Declare the mqtt config (but don't initialize it here)
extern esp_mqtt_client_config_t mqtt_cfg;

// Function to initialize MQTT configuration
esp_mqtt_client_handle_t mqtt_app_start(void);

#endif /* MQTTS_H */

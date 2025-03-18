idf_component_register(SRCS "wifi.c"
	INCLUDE_DIRS "." "../eeprom"
	REQUIRES main bt wifi_provisioning ble esp_event esp_wifi)


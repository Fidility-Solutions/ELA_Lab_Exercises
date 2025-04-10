
#idf_component_register(SRCS "data_display.c" "rtc.c" "data_collection.c" "mq135.c" "IQT.c" "bme680_sensor.c" "data_storage.c" "sds011.c"
#                       INCLUDE_DIRS "."
#		       REQUIRES bme680 nvs_flash driver esp_driver_uart esp_adc esp_driver_gpio mqtt json)
#

idf_component_register(
	SRCS "main.c"
	"data_collection.c"
	"data_display.c"
	"data_storage.c"
	"bme680_sensor.c"
	"rtc.c"
	"mqtts.c"
	"led.c"
	INCLUDE_DIRS "."
	REQUIRES esp_wifi
	nvs_flash
	esp_event
	#driver
	esp_driver_uart
	esp_driver_gpio
	esp_adc
	adc
	ble
	sds011
	spi_flash
	i2cdev
	bme680
	eeprom
	wifi
	bt
	mqtt  # ✅ Add this to link esp-mqtt correctly
	tcp_transport
	json
	#EMBED_FILES "aws_root_ca.pem" "client_cert.pem" "client_key.pem"
	)
target_compile_options(${COMPONENT_LIB} PRIVATE -ffunction-sections -fdata-sections)
target_link_options(${COMPONENT_LIB} PRIVATE -Wl,--gc-sections)
set(COMPONENT_EMBED_TXTFILES partitions.csv)

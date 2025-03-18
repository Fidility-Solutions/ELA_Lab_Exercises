idf_component_register(SRCS "ble.c"
                    INCLUDE_DIRS "." "$ENV{IDF_PATH}/components/bt/host/bluedroid/api/include"
		    REQUIRES main wifi bt esp_wifi driver eeprom  # Add this line
		    )


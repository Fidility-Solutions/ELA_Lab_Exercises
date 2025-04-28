# Register the Wi-Fi component with ESP-IDF build system.
# - Sources: wifi.c
# - Include directories: current directory (.) and ../eeprom for EEPROM integration
# - Dependencies:
#     - main: Main application logic
#     - bt: Bluetooth stack support
#     - wifi_provisioning: For provisioning Wi-Fi credentials via BLE or other means
#     - ble: Bluetooth Low Energy
#     - esp_event: ESP-IDF event loop handling
#     - esp_wifi: Core Wi-Fi functionality

idf_component_register(SRCS "wifi.c"
        INCLUDE_DIRS "." "../eeprom"
        REQUIRES main bt wifi_provisioning ble esp_event esp_wifi)


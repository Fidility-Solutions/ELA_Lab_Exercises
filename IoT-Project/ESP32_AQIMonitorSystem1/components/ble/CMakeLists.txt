# Register the BLE component with ESP-IDF build system.
# - Sources: ble.c
# - Include directories:
#     - Current directory (.)
#     - Bluedroid API include path for BLE functionalities
# - Dependencies:
#     - main     : Main application logic
#     - wifi     : For BLE-based Wi-Fi provisioning use-cases
#     - bt       : Core Bluetooth stack (Bluedroid)
#     - esp_wifi : ESP-IDF Wi-Fi stack (used for coordination with BLE provisioning)
#     - driver   : Low-level driver support (e.g., GPIO, UART, etc.)
#     - eeprom   : For persistent BLE and provisioning-related storage

idf_component_register(SRCS "ble.c"
    INCLUDE_DIRS "." "$ENV{IDF_PATH}/components/bt/host/bluedroid/api/include"
    REQUIRES main wifi bt esp_wifi driver eeprom
)


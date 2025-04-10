# BLE Component for ESP32

This component implements BLE (Bluetooth Low Energy) functionalities required for provisioning and communication in ESP32-based applications.

## Features

- BLE advertisement and GAP event handling
- BLE-based provisioning for:
  - Wi-Fi credentials
  - AWS endpoint and certificate
  - Device location
- EEPROM-based storage of received provisioning data
- Event synchronization using FreeRTOS event groups

## File Structure

ble/ 
├── ble.c 		# Main BLE source file containing initialization and event handling logic 
├── ble.h 		# BLE function declarations and data structures 
├── CMakeLists.txt	# Component registration for ESP-IDF build system 
└── README.md 		# This file


## Dependencies

This component depends on the following ESP-IDF components:

- `main` - Main application logic
- `wifi` - Used for BLE-to-Wi-Fi provisioning flow
- `bt` - ESP-IDF's Bluedroid Bluetooth stack
- `esp_wifi` - Wi-Fi stack (for provisioning integration)
- `driver` - Low-level drivers (e.g., GPIO, UART, etc.)
- `eeprom` - Custom EEPROM component for storing configuration data

## API Overview

### `void ble_init(void);`

Initializes BLE, sets advertisement data, registers GAP callbacks, and starts advertising.

### `void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);`

Handles BLE GAP events like advertisement start/stop and data configuration.

### Provisioning Parsers

- `parse_wifi_credentials()`
- `parse_aws_certificate()`
- `parse_aws_endpoint()`
- `parse_location()`

Each parser handles BLE-received data and stores the parsed values in EEPROM.

## How to Use

1. Include `ble_init()` in your application's initialization code.
2. Make sure the required components (`eeprom`, `wifi`, etc.) are available and properly initialized.
3. Use a mobile app or BLE client to send provisioning data in the expected format (e.g., `SSID,PASSWORD`).
4. The device stores the data and triggers actions like connecting to Wi-Fi or initializing MQTT.

## BLE Data Format (Expected)

- **Wi-Fi credentials:** `SSID,PASSWORD`
- **AWS Endpoint:** `your-endpoint.amazonaws.com`
- **AWS Certificate:** Raw certificate data (PEM format)
- **Location:** `LATITUDE,LONGITUDE` (e.g., `12.9716,77.5946`)

## License

This component is developed and maintained by **FidilitySolutions**. Refer to the project-level `LICENSE` for details.



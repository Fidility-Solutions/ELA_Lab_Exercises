# Wi-Fi Driver Component

## Description
This component handles the initialization and management of Wi-Fi connectivity on ESP32 in Station (STA) mode. It supports automatic reconnection, IP acquisition, and integration with MQTT. The logic also supports Wi-Fi provisioning using BLE if required.

The driver includes:
- Wi-Fi stack initialization and configuration
- Event handling for connect/disconnect/IP assignment
- Retry mechanism with system watchdog reset on failure
- MQTT client start/stop based on connection status


## Features
- Station mode Wi-Fi connection
- Auto-reconnection on failure
- IP acquisition event tracking
- MQTT client management (start/stop)
- Retry with watchdog reset on prolonged failure
- EventGroup synchronization support
- Optional support for BLE provisioning

## File Structure
wifi/
|
|---wifi.c           # Core logic for Wi-Fi init, events, reconnection, MQTT management
|
|---wifi.h           # Header file with macros, global variables, and function prototypes
|
|---CMakeLists.txt   # Registers Wi-Fi component and dependencies with ESP-IDF




# ADC Driver Component

## Description
This component provides an abstraction for initializing and using the ADC (Analog-to-Digital Converter) functionality on the ESP32 using ESP-IDF’s One-Shot ADC Driver. It is particularly useful for interfacing with analog sensors like the MQ-135 Air Quality Sensor.

## Features
- One-shot ADC initialization using adc_oneshot
- Configurable channel and attenuation
- Function to read analog values from the configured ADC channel
- Error handling and logging using ESP LOGx macros

## File Structure
adc/
│
├── adc.c             # Source file with ADC initialization and read functionality
├── adc.h             # Header file declaring public ADC APIs
├── CMakeLists.txt    # CMake file for ESP-IDF build system
└── README.md         # This documentation file


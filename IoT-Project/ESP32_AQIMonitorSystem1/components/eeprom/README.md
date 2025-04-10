# EEPROM Driver Component

## Description

This component provides a simple EEPROM driver that supports **read** and **write** operations using I2C communication. It is designed to interface with standard EEPROM chips and includes test routines to validate functionality.

The driver includes:
- Initialization of the I2C interface
- Byte and block read/write operations
- Self-test function to verify EEPROM integrity

## Features

- I2C-based communication
- Read and write support for arbitrary addresses
- Support for multiple EEPROM sizes (configurable)
- Optional delay handling for write cycles
- Easy integration with FreeRTOS or bare-metal systems

## File Structure
eeprom/
|
|---eeprom.c         # Source file implementing EEPROM read/write functionalities
|
|---eeprom.h         # Header file declaring EEPROM APIs (function prototypes, macros, etc.)
|
|---CMakeLists.txt   # CMake configuration file to register the EEPROM component with ESP-IDF


# SDS011 Air Quality Sensor Component

## Description

This component provides an interface for the **SDS011** particulate matter (PM2.5 and PM10) sensor using the **ESP-IDF** framework. It supports UART communication to read air quality data and calculate the corresponding **Air Quality Index (AQI)**.


## Features

- UART communication with SDS011 sensor
- Real-time reading of PM2.5 and PM10 data
- AQI calculation based on CPCB India breakpoints
- Easy integration with other ESP-IDF components


## File Structure
sds011/
|
|--> sds011.c   	# Implementation of SDS011 UART initialization, data read, and AQI calculation 
|--> sds011.h   	# Header file with data structures, macros, and function prototypes 
|--> CMakeLists.txt 	# Component registration in ESP-IDF 


#
# CMakeLists.txt - Component configuration for ADC driver
#
# Description:
# This file registers the ADC component with the ESP-IDF build system. It specifies the source file,
# include directories, and required dependent components.
#
# Component Name : adc
# Source Files   : adc.c
# Include Dirs   : Current directory (.)
# Requirements   : Depends on esp_adc (ESP-IDF ADC driver) and main application
#
# Usage:
# Include this component in your project to perform ADC operations like initialization,
# reading analog values (e.g., from sensors like MQ-135), and integrating with application logic.
#

idf_component_register(SRCS "adc.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_adc main)


#
# CMakeLists.txt - Component configuration for SDS011 sensor interface
#
# Description:
#   This file registers the SDS011 air quality sensor module as an ESP-IDF component.
#   It specifies the source files, include directories, and dependencies required
#   for proper integration within the ESP-IDF build system.
#
#   - SRCS         : Source file(s) implementing the SDS011 functionality.
#   - INCLUDE_DIRS : Public header file directory (current directory in this case).
#   - REQUIRES     : Specifies the component dependency on the UART driver (esp_driver_uart).
#

idf_component_register(SRCS "sds011.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_uart)


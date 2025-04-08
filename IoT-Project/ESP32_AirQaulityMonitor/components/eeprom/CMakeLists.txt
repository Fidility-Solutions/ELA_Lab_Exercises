# CMakeLists.txt - ESP-IDF component build script for EEPROM driver

# Register the EEPROM component with ESP-IDF build system
idf_component_register(
    SRCS "eeprom.c"           # Source file that contains EEPROM implementation
    INCLUDE_DIRS "."          # Public include path (makes eeprom.h accessible to other components)
    REQUIRES driver           # Declares dependency on ESP-IDF's 'driver' component (e.g., I2C driver)
)


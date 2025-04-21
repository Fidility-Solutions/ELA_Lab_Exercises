# SPI Flash Driver Component for ESP32

This component provides an SPI driver to interface with external SPI flash devices. It includes basic operations such as:

- Initializing the SPI bus and device
- Transmitting data
- Writing and erasing sectors
- Reading data from flash
- Ensuring write/erase completion via polling

## Files

- `spi.c`: Implementation of SPI functions.
- `spi.h`: Header file with public APIs and constants.
- `CMakeLists.txt`: CMake file for component integration.
- `README.md`: This documentation.

## Configuration

Make sure to define your GPIO connections in `spi.h`:
```c
#define GPIO_MOSI 23
#define GPIO_MISO 19
#define GPIO_SCLK 18
#define GPIO_CS    5


#include "main.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Define GPIO pins for the RGB LED
#define RED_GPIO 25
#define GREEN_GPIO 26
#define BLUE_GPIO 27



// Function to initialize RGB LED GPIOs
void rgb_led_init() {
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RED_GPIO) | (1ULL << GREEN_GPIO) | (1ULL << BLUE_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&io_conf);
}

// Function to control the RGB LED based on AQI value
void control_rgb_led(int aqi) {
    if (aqi <= 50) {
        // AQI is good, turn on Green
        gpio_set_level(RED_GPIO, 0);
        gpio_set_level(GREEN_GPIO, 1);
        gpio_set_level(BLUE_GPIO, 0);
        printf("AQI: %d, Green LED ON\n", aqi);
    } else if (aqi > 50 && aqi <= 100) {
        // AQI is moderate, turn on Yellow (Red + Green)
        gpio_set_level(RED_GPIO, 1);
        gpio_set_level(GREEN_GPIO, 1);
        gpio_set_level(BLUE_GPIO, 0);
        printf("AQI: %d, Yellow LED ON\n", aqi);
    } else {
        // AQI is poor or unhealthy, turn on Red
        gpio_set_level(RED_GPIO, 1);
        gpio_set_level(GREEN_GPIO, 0);
        gpio_set_level(BLUE_GPIO, 0);
        printf("AQI: %d, Red LED ON\n", aqi);
    }
}


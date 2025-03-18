#include "driver/adc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define MQ135_PIN ADC1_CHANNEL_0
#define ADC_WIDTH ADC_WIDTH_BIT_12        // 12-bit ADC resolution (0-4095)
#define ADC_ATTENUATION ADC_ATTEN_DB_0    // 0 to 1V range for input

#define TAG "ADC_TAG"


void mq_135(void)
{
    esp_err_t ret = adc1_config_width(ADC_WIDTH);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure ADC width");
        vTaskDelete(NULL); // If setup fails, delete the task
    }

    // Configure ADC attenuation for input voltage range 0 to 1V
    ret = adc1_config_channel_atten(MQ135_PIN, ADC_ATTENUATION);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to configure ADC channel attenuation");
        vTaskDelete(NULL); // If setup fails, delete the task
    }
}

#include "driver/spi_master.h"
#include "main.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <unistd.h>


#define GPIO_MOSI           23
#define GPIO_MISO           19
#define GPIO_SCLK           18
#define GPIO_CS             5

#define ESP_HOST VSPI_HOST
esp_err_t ESP_ERR;
spi_device_handle_t spi_handle;
esp_err_t ret;

// Mutex for SPI synchronization
SemaphoreHandle_t spi_mutex;

void SPI_Receive(uint16_t cmd, size_t cmdLen, uint64_t Addr, size_t addrLen, uint8_t *buffertoStore, size_t dataLen);
void sector_erase(int numsector);
void spi_transmit(uint8_t *data, int bytes);
void write_data (int pagenum, uint8_t *datatoWrite, size_t bytes);

#define MAX_ADDRESS 0x7FFFFF
static int counter = 16;

// SPI initialization
esp_err_t SPI_Init(void) {
    // Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1
    };

    // Configuration for the SPI device
    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .clock_speed_hz = 2000000,
        .duty_cycle_pos = 128, // 50% duty cycle
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 3
    };

    ret = spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to initialize bus: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Failed to add device: %s", esp_err_to_name(ret));
        return ret;
    }

    return ESP_OK;
}

void spi_transmit(uint8_t *data, int bytes) {
    spi_transaction_t trans;
    memset(&trans, 0, sizeof(spi_transaction_t));

    trans.tx_buffer = data;
    trans.length = bytes * 8;

    if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) { // Take SPI mutex
        if (spi_device_transmit(spi_handle, &trans) != ESP_OK) {
            printf("Writing error\n");
        }
        xSemaphoreGive(spi_mutex); // Release SPI mutex
    }
}

void write_enable(void) {
    uint8_t cmd = 0x06;
    spi_transmit(&cmd, 1);
    usleep(5000); // Wait 5ms
}

void wait_for_write_completion() {
    uint8_t cmd = 0x05; // Read Status Register
    uint8_t status;

    do {
        spi_transmit(&cmd, 1);

        spi_transaction_t trans;
        memset(&trans, 0, sizeof(trans));
        trans.rx_buffer = &status;
        trans.length = 8; // 1 byte

        if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) { // Take SPI mutex
            spi_device_transmit(spi_handle, &trans);
            xSemaphoreGive(spi_mutex); // Release SPI mutex
        }
    } while (status & 0x01); // WIP bit
}

void sector_erase(int numsector) {
    uint8_t cmd = 0x20;
    uint32_t memAddress = numsector * 16 * 256; // Sector num x 16 pages x 256 bytes
    uint8_t tData[4] = { cmd, (memAddress >> 16) & 0xFF, (memAddress >> 8) & 0xFF, memAddress & 0xFF };

    write_enable();
    spi_transmit(tData, 4);
    wait_for_write_completion();
}

void read_data(int pagenum, uint8_t *buffertoStore, size_t bytes) {
    uint8_t cmd = 0x03;
    uint32_t memAddress = pagenum * 256;
    SPI_Receive(cmd, 1, memAddress, 3, buffertoStore, bytes);
}

void SPI_Receive(uint16_t cmd, size_t cmdLen, uint64_t Addr, size_t addrLen, uint8_t *buffertoStore, size_t dataLen) {
    spi_transaction_t trans;
    memset(&trans, 0, sizeof(trans));

    spi_transaction_ext_t trans_ext;
    memset(&trans_ext, 0, sizeof(trans_ext));

    trans.flags = SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_CMD;
    trans.cmd = cmd;
    trans.addr = Addr;
    trans.rx_buffer = buffertoStore;
    trans.length = dataLen * 8;

    trans_ext.address_bits = addrLen * 8;
    trans_ext.command_bits = cmdLen * 8;
    trans_ext.base = trans;

    if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) { // Take SPI mutex
        if (spi_device_transmit(spi_handle, (spi_transaction_t *)&trans_ext) != ESP_OK) {
            printf("Writing ERROR\n");
        }
        xSemaphoreGive(spi_mutex); // Release SPI mutex
    }
}

void write_data(int pagenum, uint8_t *datatoWrite, size_t bytes) {
    uint8_t cmd = 0x02;
    uint32_t memAddress = pagenum * 256;
    uint8_t tData[bytes + 4]; // 4 additional bytes -> 1 cmd, 3 addr
    tData[0] = cmd;
    tData[1] = (memAddress >> 16) & 0xFF;
    tData[2] = (memAddress >> 8) & 0xFF;
    tData[3] = memAddress & 0xFF;

    for (int i = 0; i < bytes; i++) {
        tData[i + 4] = datatoWrite[i];
    }

    write_enable();
    spi_transmit(tData, bytes + 4);
    usleep(200000);
}

void dataStorageTask(void *pvParameters) {
    // Create SPI mutex
    spi_mutex = xSemaphoreCreateMutex();
    if (spi_mutex == NULL) {
        ESP_LOGE("SPI", "Failed to create SPI mutex");
        return;
    }

    // Initialize SPI
    ret = SPI_Init();
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "SPI Initialization failed");
        return;
    }

    while (1) {
       if (xQueuePeek(sensorDataQueue, &global_sensor_data, portMAX_DELAY) == pdTRUE) 
       {
            if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY)) 
	    {
          //      int sector_num = counter / 16; // Sector number
            //    int page_offset = counter % 16; // Page offset in the sector

	        uint32_t sector_num = counter / (16 * 256); // Sector size = 16 pages x 256 bytes
                uint32_t page_offset = (counter % (16 * 256)) / 256; // Page offset within the sector


                if (page_offset == 0) {
                    sector_erase(sector_num);
                    vTaskDelay(pdMS_TO_TICKS(200));
                }

                write_data(counter/256, (uint8_t *)&global_sensor_data, sizeof(global_sensor_data));
		ESP_LOGI("STORAGE", "\033[34mStoring: Temp = %.2f°C, Hum = %.2f%%, CO2 = %.2f ppm," 
				"AQI = %d, time is %s\033[0m",global_sensor_data.bme680.temperature, 
				global_sensor_data.bme680.humidity, global_sensor_data.bme680.gas_resistance, 
				global_sensor_data.air_quality_index, global_sensor_data.time_str);

                read_data(counter/256, (uint8_t *)&processed_data, sizeof(processed_data));
		ESP_LOGI("READING", "Stored: Temp = %.2f°C, Hum = %.2f%%, CO2 = %.2f ppm, AQI = %d, time is %s",
	       	processed_data.bme680.temperature, processed_data.bme680.humidity, processed_data.bme680.gas_resistance, 
		processed_data.air_quality_index, processed_data.time_str);

		counter += 256; // Increment by one page (256 bytes)
                if (counter > MAX_ADDRESS) {
                    counter = 16; // Wrap around to the starting address
                }
               // counter++;
               // if (counter >= max_writes) {
                 //   counter = 16;
               // }

                xSemaphoreGive(dataSyncSemaphore);
            }
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

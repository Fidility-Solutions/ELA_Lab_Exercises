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

#define MAX_ADDRESS 0x7FFFFF

esp_err_t ESP_ERR;

spi_device_handle_t spi_handle;

esp_err_t ret;

extern esp_mqtt_client_handle_t client;

/* Mutex for SPI synchronization */
SemaphoreHandle_t spi_mutex;

void SPI_Receive(uint16_t cmd, size_t cmdLen, uint64_t Addr, size_t addrLen, uint8_t *buffertoStore, size_t dataLen);

void sector_erase(int numsector);

void spi_transmit(uint8_t *data, int bytes);

void write_data (int pagenum, uint8_t *datatoWrite, size_t bytes);


static int counter = 16;

static int cloud_counter = 16;


/*
 * This task handles the process of storing sensor data to SPI flash when Wi-Fi is unavailable.
 * Once Wi-Fi is restored, it sends the stored data to the cloud.
 *
 * Key Steps:
 * 1. If Wi-Fi is disconnected:
 *    - Lock the shared data using a semaphore to prevent concurrent access.
 *    - Copy the current sensor data.
 *    - Calculate the storage location in SPI flash.
 *    - Erase the corresponding sector of the SPI flash if needed.
 *    - Write the sensor data to the flash.
 *    - Increment the counter to prepare for the next write operation.
 *    - Release the semaphore after storing the data.
 * 2. If Wi-Fi is connected:
 *    - Check if there is stored data that needs to be sent to the cloud.
 *    - Read the stored data from SPI flash.
 *    - Publish the data to the cloud via MQTT.
 *    - Once all data is uploaded, reset the counters.
 * 3. Task Delay: Wait for a short period before the next cycle to prevent overloading the system.
 */

/*
 * This function should be responsible for reading sensor data, processing it, and handling its storage or transmission
 * based on the Wi-Fi connectivity status.
 */


/*void dataStorageTask(void *pvParameters) 
{
    spi_mutex = xSemaphoreCreateMutex();
    if (spi_mutex == NULL) 
    {
        ESP_LOGE("SPI", "Failed to create SPI mutex");
        return;
    }

    ret = SPI_Init();
    if (ret != ESP_OK) 
    {
        ESP_LOGE("SPI", "SPI Initialization failed");
        return;
    }

    STR_SENSOR_DATA last_sent_data = {0}; // Track last sent data
    STR_SENSOR_DATA local_copy;

    while (1)
    {
        // Check if Wi-Fi is disconnected (store locally)
        if ((gs8wificonnectedflag == 0) && (gs8initialconnectionflag == 1))
        {
            if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
            {
                memcpy(&local_copy, &global_sensor_data, sizeof(STR_SENSOR_DATA));  // Copy safely

                // **Check if data is new before storing**
                if (memcmp(&last_sent_data, &local_copy, sizeof(STR_SENSOR_DATA)) != 0)
                {
                    memcpy(&last_sent_data, &local_copy, sizeof(STR_SENSOR_DATA)); // Update last sent data
                    
                    // Compute storage location
                    uint32_t sector_num = counter / (16 * 256);
                    uint32_t page_offset = (counter % (16 * 256)) / 256;

                    // Erase sector if needed
                    if (page_offset == 0)
                    {
                        sector_erase(sector_num);
                        vTaskDelay(pdMS_TO_TICKS(200)); // Allow sector erase to complete
                    }

                    // Write to storage
                    write_data(counter / 256, (uint8_t *)&local_copy, sizeof(local_copy));

                    // Increment counter and handle wrap-around
                    counter += 256; // Increment by one page
                    if (counter > MAX_ADDRESS)
                    {
                        counter = 16; // Wrap around
                    }
                }
                
                xSemaphoreGive(dataSyncSemaphore); // Release semaphore
            }
        }
        else if (cloud_counter < counter) // Wi-Fi connected, upload stored data
        {
            read_data(cloud_counter / 256, (uint8_t *)&processed_data, sizeof(processed_data));
            publish_sensor_data(&processed_data, client);

            cloud_counter += 256; // Increment

            if (cloud_counter >= counter)
            {
                ESP_LOGI("RESET", "All data uploaded. Resetting counters.");
                cloud_counter = 16;
                counter = 16;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));  // General delay
    }
}*/



/* SPI initialization */
esp_err_t SPI_Init(void) {
	/* Configuration for the SPI bus */
	spi_bus_config_t buscfg = {
		.mosi_io_num = GPIO_MOSI,
		.miso_io_num = GPIO_MISO,
		.sclk_io_num = GPIO_SCLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1
	};

	/* Configuration for the SPI device */
	spi_device_interface_config_t devcfg = 
	{
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.clock_speed_hz = 2000000,
		.duty_cycle_pos = 128, /* 50% duty cycle */
		.mode = 0,
		.spics_io_num = GPIO_CS,
		.queue_size = 3
	};

	ret = spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

	if (ret != ESP_OK) 
	{
		ESP_LOGE("SPI", "Failed to initialize bus: %s", esp_err_to_name(ret));
		return ret;
	}

	ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);

	if (ret != ESP_OK) 
	{
		ESP_LOGE("SPI", "Failed to add device: %s", esp_err_to_name(ret));
		return ret;
	}

	return ESP_OK;
}

void spi_transmit(uint8_t *data, int bytes) 
{
	spi_transaction_t trans;
	memset(&trans, 0, sizeof(spi_transaction_t));

	trans.tx_buffer = data;
	trans.length = bytes * 8;

	if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) 
	{ 
		/* Take SPI mutex */
		if (spi_device_transmit(spi_handle, &trans) != ESP_OK) 
		{
			printf("Writing error\n");
		}
		xSemaphoreGive(spi_mutex); /* Release SPI mutex */
	}
}

void write_enable(void) 
{
	uint8_t cmd = 0x06;
	spi_transmit(&cmd, 1);
	usleep(5000); /* Wait 5ms */
}

void wait_for_write_completion() 
{
	uint8_t cmd = 0x05; /* Read Status Register */
	uint8_t status;

	do 
	{
		spi_transmit(&cmd, 1);

		spi_transaction_t trans;
		memset(&trans, 0, sizeof(trans));
		trans.rx_buffer = &status;
		trans.length = 8; // 1 byte

		if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) 
		{ 
			/* Take SPI mutex */
			spi_device_transmit(spi_handle, &trans);
			xSemaphoreGive(spi_mutex); /* Release SPI mutex */
		}
	} while (status & 0x01); /* WIP bit */
}

void sector_erase(int numsector) 
{
	uint8_t cmd = 0x20;
	uint32_t memAddress = numsector * 16 * 256; /* Sector num x 16 pages x 256 bytes */
	uint8_t tData[4] = { cmd, (memAddress >> 16) & 0xFF, (memAddress >> 8) & 0xFF, memAddress & 0xFF };

	write_enable();
	spi_transmit(tData, 4);
	wait_for_write_completion();
}

void read_data(int pagenum, uint8_t *buffertoStore, size_t bytes) 
{
	uint8_t cmd = 0x03;
	uint32_t memAddress = pagenum * 256;
	SPI_Receive(cmd, 1, memAddress, 3, buffertoStore, bytes);
}

void SPI_Receive(uint16_t cmd, size_t cmdLen, uint64_t Addr, size_t addrLen, uint8_t *buffertoStore, size_t dataLen) 
{
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

	if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) 
	{ 
		/* Take SPI mutex */
		if (spi_device_transmit(spi_handle, (spi_transaction_t *)&trans_ext) != ESP_OK) 
		{
			printf("Writing ERROR\n");
		}
		xSemaphoreGive(spi_mutex); /* Release SPI mutex */
	}
}

void write_data(int pagenum, uint8_t *datatoWrite, size_t bytes) 
{
	uint8_t cmd = 0x02;
	uint32_t memAddress = pagenum * 256;
	uint8_t tData[bytes + 4]; /* 4 additional bytes -> 1 cmd, 3 addr */
	tData[0] = cmd;
	tData[1] = (memAddress >> 16) & 0xFF;
	tData[2] = (memAddress >> 8) & 0xFF;
	tData[3] = memAddress & 0xFF;

	for (int i = 0; i < bytes; i++) 
	{
		tData[i + 4] = datatoWrite[i];
	}

	write_enable();
	spi_transmit(tData, bytes + 4);
	usleep(200000);
}

void dataStorageTask(void *pvParameters)
{
    spi_mutex = xSemaphoreCreateMutex();
    if (spi_mutex == NULL)
    {
        ESP_LOGE("SPI", "Failed to create SPI mutex");
        return;
    }

    ret = SPI_Init();
    if (ret != ESP_OK)
    {
        ESP_LOGE("SPI", "SPI Initialization failed");
        return;
    }

    STR_SENSOR_DATA last_sent_data = {0}; // Track last sent data
    STR_SENSOR_DATA local_copy;

    while (1)
    {
        // Check if Wi-Fi is disconnected (store locally)
        if ((gs8wificonnectedflag == 0) && (gs8initialconnectionflag == 1))
        {
            if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
            {
                memcpy(&local_copy, &global_sensor_data, sizeof(STR_SENSOR_DATA));  // Copy safely

                // **Check if data is new before storing**
                if (memcmp(&last_sent_data, &local_copy, sizeof(STR_SENSOR_DATA)) != 0)
                {
                    memcpy(&last_sent_data, &local_copy, sizeof(STR_SENSOR_DATA)); // Update last sent data

                    // Compute storage location
                    uint32_t sector_num = counter / (16 * 256);
                    uint32_t page_offset = (counter % (16 * 256)) / 256;

                    // Erase sector if needed
                    if (page_offset == 0)
                    {
                        sector_erase(sector_num);
                        vTaskDelay(pdMS_TO_TICKS(200)); // Allow sector erase to complete
                    }

                    // Write to storage
                    write_data(counter / 256, (uint8_t *)&local_copy, sizeof(local_copy));

                    // Increment counter and handle wrap-around
                    counter += 256; // Increment by one page
                    if (counter > MAX_ADDRESS)
                    {
                        counter = 16; // Wrap around
                    }
                }

                xSemaphoreGive(dataSyncSemaphore); // Release semaphore
            }
        }
        else if (cloud_counter < counter) // Wi-Fi connected, upload stored data
        {
            read_data(cloud_counter / 256, (uint8_t *)&processed_data, sizeof(processed_data));
            publish_sensor_data(&processed_data, client);

            cloud_counter += 256; // Increment

            if (cloud_counter >= counter)
            {
                ESP_LOGI("RESET", "All data uploaded. Resetting counters.");
                cloud_counter = 16;
                counter = 16;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(500));  // General delay
    }
}


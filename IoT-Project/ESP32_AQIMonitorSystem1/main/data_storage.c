/* Header files */
#include "data_storage.h"
#include "main.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"

spi_device_handle_t spi_handle;

extern esp_mqtt_client_handle_t mqtt_client_handle;
STR_SENSOR_DATA str_processed_data;
/* Mutex for SPI synchronization */
SemaphoreHandle_t spi_mutex;

static uint32_t u32Counter = COUNTER_INITIAL_VALUE;  		/* Current storage position */
static uint32_t u32CloudCounter = CLOUD_COUNTER_INITIAL_VALUE;  	/* Current cloud data upload position */
/*
 * Function     : SPI_Init()
 *
 * Description  : This function initializes the SPI bus and adds an SPI device to it. It sets up the configuration 
 * 		for both the SPI bus and the SPI device. The bus configuration includes setting up the MOSI, MISO, 
 * 		SCLK, and CS pins. The device configuration includes specifying the SPI mode, clock speed, duty cycle, 
 * 		and the size of the command/address fields. It also initializes the SPI bus and adds the SPI device 
 * 		to the bus, using a handle (`spi_handle`) for communication with the device.
 *
 * Parameters   : None
 *
 * Returns      : esp_err_t - Returns `ESP_OK` if initialization is successful, otherwise returns an error code.
 *                If initialization fails, error details are logged using `ESP_LOGE`.
 *
 */
esp_err_t SPI_Init(void) 
{
	esp_err_t eErrStat;
	/* Configuration for the SPI bus */
	spi_bus_config_t buscfg = 
	{
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

	eErrStat = spi_bus_initialize(VSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

	if (eErrStat != ESP_OK) 
	{
		ESP_LOGE("SPI", "Failed to initialize bus: %s", esp_err_to_name(eErrStat));
		return eErrStat;
	}

	eErrStat = spi_bus_add_device(VSPI_HOST, &devcfg, &spi_handle);

	if (eErrStat != ESP_OK) 
	{
		ESP_LOGE("SPI", "Failed to add device: %s", esp_err_to_name(eErrStat));
		return eErrStat;
	}

	return ESP_OK;
}

/*
 * Function     : spi_transmit()
 *
 * Description  : This function performs an SPI data transmission by sending a specified number of bytes over the 
 * 		SPI interface. It configures the SPI transaction using the provided data buffer (`pu8Data`) and 
 * 		the number of bytes to be transmitted (`u32Bytes`).
 *              A mutex (`spi_mutex`) is used to ensure that only one task can transmit data over SPI at a time, 
 *              preventing race conditions. The function blocks the calling task until the SPI transaction is 
 *              completed. If the transmission fails, an error message is printed.
 *
 * Parameters   :
 *      pu8Data  - A pointer to the data buffer containing the bytes to be transmitted.
 *      u32Bytes - The number of bytes to be transmitted (in bytes).
 *
 * Returns      : None
 *
 */
void spi_transmit(uint8_t *pu8Data, uint32_t u32Bytes) 
{
	spi_transaction_t trans;
	memset(&trans, 0, sizeof(spi_transaction_t));

	trans.tx_buffer = pu8Data;
	trans.length = u32Bytes * 8;

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

/*
 * Function     : write_enable()
 *
 * Description  : Sends the Write Enable command (0x06) to the SPI flash memory.
 *                This sets the Write Enable Latch (WEL) bit in the status register,
 *                which is required before performing any write, erase, or program operations.
 *
 * Parameters   : None
 *
 * Returns      : None
 */
void write_enable(void) 
{
	uint8_t u8Cmd = WRITE_ENABLE_CMD;
	spi_transmit(&u8Cmd, 1); /* Use a compound literal to pass the value */
	vTaskDelay(pdMS_TO_TICKS(5));
}


/*
 * Function     : wait_for_write_completion()
 *
 * Description  : Waits until the SPI Flash completes an ongoing write, erase,
 *                or program operation. It continuously polls the status register's
 *                WIP (Write-In-Progress) bit and waits until it is cleared.
 *                Uses a semaphore to ensure thread-safe SPI access.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 */
void wait_for_write_completion(void) 
{
	uint8_t u8Stat;
	uint8_t u8Cmd = READ_STATUS_REG_CMD;


	do 
	{
		spi_transmit(&u8Cmd, 1);

		spi_transaction_t trans;
		memset(&trans, 0, sizeof(trans));
		trans.rx_buffer = &u8Stat;
		trans.length = 8; /* 1 byte */

		if (xSemaphoreTake(spi_mutex, portMAX_DELAY)) 
		{ 
			/* Take SPI mutex */
			spi_device_transmit(spi_handle, &trans);
			xSemaphoreGive(spi_mutex); /* Release SPI mutex */
		}
	} while (u8Stat & WIP_BIT_MASK); /* WIP bit */
}

/*
 * Function     : sector_erase()
 *
 * Description  : This function erases a specific sector in the SPI flash memory.
 *                Each sector is assumed to be 4KB in size (16 pages x 256 bytes).
 *                It sends the sector erase command (0x20) along with the calculated
 *                24-bit memory address corresponding to the sector number.
 *                It ensures write enable is set before issuing the erase command and
 *                waits for the erase operation to complete.
 *
 * Parameters   : u8NumSector - Sector number to be erased. The memory address is derived
 *                               based on this number.
 *
 * Returns      : None
 */
void sector_erase(uint8_t u8NumSector) 
{
	uint32_t u32MemAddr = u8NumSector * 16 * 256; /* Sector num x 16 pages x 256 bytes */
	uint8_t u8TxData[4] = { CMD_SECTOR_ERASE, (u32MemAddr >> 16) & 0xFF, 
		(u32MemAddr >> 8) & 0xFF, u32MemAddr & 0xFF };

	write_enable();
	spi_transmit(u8TxData, 4);
	wait_for_write_completion();
}

/*
 * Function     : read_data()
 *
 * Description  : Reads a specified number of bytes from a page in the SPI Flash memory.
 *                Calculates the memory address based on the given page number and reads
 *                data starting from that address into the provided buffer.
 *
 * Parameters   : u8PageNum  - Page number to read from (each page is 256 bytes)
 *                pu8Buffer  - Pointer to the buffer where the read data will be stored
 *                u32Bytes   - Number of bytes to read from the flash memory
 *
 * Returns      : None
 *
 */
void read_data(uint8_t u8PageNum, uint8_t *pu8Buffer, uint32_t u32Bytes) 
{
	uint32_t u32MemAddr = u8PageNum * 256;

	SPI_Receive(CMD_READ_DATA, 1, u32MemAddr, 3, pu8Buffer, u32Bytes);
}

void SPI_Receive(uint16_t u16Cmd, size_t u16CmdLen, uint64_t u64Addr, uint32_t u32AddrLen, 
		uint8_t *pu8Buffer, uint32_t u32BufLen)
{
	spi_transaction_t trans;
	memset(&trans, 0, sizeof(trans));

	spi_transaction_ext_t trans_ext;
	memset(&trans_ext, 0, sizeof(trans_ext));

	trans.flags = SPI_TRANS_VARIABLE_ADDR | SPI_TRANS_VARIABLE_CMD;
	trans.cmd = u16Cmd;
	trans.addr = u64Addr;
	trans.rx_buffer = pu8Buffer;
	trans.length = u32BufLen * 8;

	trans_ext.address_bits = u32AddrLen * 8;
	trans_ext.command_bits = u16CmdLen * 8;
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

/*
 * Function     : write_data()
 *
 * Description  : Writes a specified number of bytes to a page in the SPI Flash memory.
 *                Constructs a transmit buffer containing the write command, memory address,
 *                and the actual data to be written. It then transmits this buffer over SPI
 *                after enabling write operations.
 *
 * Parameters   : u8PageNum       - Page number to write to (each page is 256 bytes)
 *                pu8DataToWrite  - Pointer to the data to be written to the flash memory
 *                u32Bytes        - Number of bytes to write
 *
 * Returns      : None
 *
 */
void write_data(uint8_t u8PageNum, uint8_t *pu8DataToWrite, uint32_t u32Bytes) 
{
	uint32_t u32MemAddr = u8PageNum * 256;
	uint8_t u8TxData[u32Bytes + 4]; /* 4 additional bytes -> 1 cmd, 3 addr */
	u8TxData[0] = WRITE_CMD;
	u8TxData[1] = (u32MemAddr >> 16) & 0xFF;
	u8TxData[2] = (u32MemAddr >> 8) & 0xFF;
	u8TxData[3] = u32MemAddr & 0xFF;

	for (int i = 0; i < u32Bytes; i++) 
	{
		u8TxData[i + 4] = pu8DataToWrite[i];
	}

	write_enable();
	spi_transmit(u8TxData, u32Bytes + 4);
	vTaskDelay(pdMS_TO_TICKS(5));
}

/*
 * Function     : dataStorageTask()
 *
 * Description  : This FreeRTOS task is responsible for handling sensor data storage and upload operations. It performs the following tasks:
 *                1. Initializes the SPI interface and creates a mutex to protect SPI operations.
 *                2. Monitors the Wi-Fi connection status to determine whether to store data locally or upload it to the cloud.
 *                3. If Wi-Fi is disconnected, the task stores sensor data locally in non-volatile memory (e.g., SPI flash), ensuring that only new data is stored.
 *                   It also handles sector erasure and data storage in 256-byte pages.
 *                4. If Wi-Fi is connected and there is stored data, the task uploads the data to the cloud by reading it from storage and publishing it via MQTT.
 *                5. It also handles wrap-around logic for the storage COUNTER_INITIAL_VALUEs to ensure that stored data does not overflow the available memory space.
 *                6. A semaphore is used to ensure synchronization when accessing shared sensor data, preventing race conditions.
 *
 * Parameters   : pvParameters - A pointer to parameters passed during task creation, not used here.
 *
 * Returns      : None
 *
 */
void dataStorageTask(void *pvParameters)
{ 
	esp_err_t eErrStat;
	spi_mutex = xSemaphoreCreateMutex();
	if (spi_mutex == NULL)
	{
		ESP_LOGE(SPI_TAG, "Failed to create SPI mutex");
		return;
	}

	eErrStat = SPI_Init();
	if (eErrStat != ESP_OK)
	{
		ESP_LOGE(SPI_TAG, "SPI Initialization failed");
		return;
	}

	STR_SENSOR_DATA last_sent_sensor_data = {0}; /* Track last sent data */
	STR_SENSOR_DATA str_local_sensor_copy;

	while (1)
	{
		/* Check if Wi-Fi is disconnected (store locally) */
		if ((gu8wificonnectedflag == 0) && (gu8initialconnectionflag == 1))
		{
			if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
			{
				memcpy(&str_local_sensor_copy, &str_global_sensor_data, sizeof(STR_SENSOR_DATA));

				/* Check if data is new before storing */
				if (memcmp(&last_sent_sensor_data, &str_local_sensor_copy, sizeof(STR_SENSOR_DATA)) != 0)
				{
					memcpy(&last_sent_sensor_data, &str_local_sensor_copy, sizeof(STR_SENSOR_DATA));

					/* Compute storage location */
					uint32_t u32SecNum = u32Counter / (16 * 256);
					uint32_t u32PageOffset = (u32Counter % (16 * 256)) / 256;

					/* Erase sector if needed */
					if (u32PageOffset == 0)
					{
						sector_erase(u32SecNum);
						vTaskDelay(pdMS_TO_TICKS(200));
					}

					/* Write to storage */
					write_data(u32Counter / 256, (uint8_t *)&str_local_sensor_copy, sizeof(str_local_sensor_copy));

					/* Increment COUNTER_INITIAL_VALUE and handle wrap-around */
					u32Counter += 256; /* Increment by one page */
					if (u32Counter > MAX_ADDRESS)
					{
						/* Wrap around */
						u32Counter = 16;
					}
				}

				/* Release semaphore */
				xSemaphoreGive(dataSyncSemaphore);
			}
		}
		else if (u32CloudCounter < u32Counter) /* Wi-Fi connected, upload stored data */
		{
			read_data(u32CloudCounter / 256, (uint8_t *)&str_processed_data, sizeof(str_processed_data));
			publish_sensor_data(&str_processed_data, mqtt_client_handle);

			u32CloudCounter += 256; /* Increment */

			if (u32CloudCounter >= u32Counter)
			{
				ESP_LOGI(SPI_TAG, "All data uploaded. Resetting COUNTER_INITIAL_VALUEs.");
				u32CloudCounter = 16;
				u32Counter = 16;
			}
		}

		vTaskDelay(pdMS_TO_TICKS(500));  /* General delay */
	}
}


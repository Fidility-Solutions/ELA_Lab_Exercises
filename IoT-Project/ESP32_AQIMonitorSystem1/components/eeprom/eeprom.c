/*
 * File:        eeprom.c
 *
 * Description: This file implements EEPROM driver functions for the ESP32 using I2C communication. 
 *              It provides APIs to initialize the I2C interface, detect EEPROM presence, and perform 
 *              byte-wise and page-wise read/write operations with proper memory addressing and delay 
 *              handling for EEPROM internal write cycles. This is suitable for external I2C EEPROM chips 
 *              like 24LC256 with 16-bit addressing.
 *
 * Usage:       Include "eeprom.h" in your application and call the APIs such as i2c_master_init(), 
 *              eeprom_write(), eeprom_read(), and detect_eeprom() as needed. Make sure to configure 
 *              I2C pins and EEPROM address in eeprom.h.
 *
 * Author:      FidilitySolutions
 *
 * Reference:   ESP-IDF I2C documentation, EEPROM datasheets (e.g., Microchip 24LC256)
 */



/* Include Source file Header */
#include "eeprom.h"

#include <driver/i2c.h>

/*
 * Function     : i2c_master_init()
 *
 * Description  : Initializes the I2C master interface (I2C_NUM_0) using the ESP-IDF framework.
 *                It sets the configuration parameters such as mode, SDA/SCL GPIO pins,
 *                pull-up settings, and clock speed. Then it installs the I2C driver.
 *                If any step fails, it logs an error message and returns the error code.
 *
 * Parameters   : None
 *  
 * Returns      : esp_err_t
 *                - ESP_OK on success
 *                - Appropriate error code (esp_err_t) on failure
 *
 */
esp_err_t i2c_master_init(void)
{
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};
	
	/* Apply I2C configuration */
	esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
	if(err != ESP_OK)
	{
		ESP_LOGE(EEPROM_TAG, "I2C parameter configuration failed: %s", esp_err_to_name(err));
		return err;
	}

	/* Install I2C driver */
	err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	if(err != ESP_OK) 
	{
		ESP_LOGE(EEPROM_TAG, "I2C driver installation failed: %s", esp_err_to_name(err));
		return err;
	}

	ESP_LOGI(EEPROM_TAG, "I2C initialized successfully");
	
	return ESP_OK;
}



/*
 * Function    : eeprom_write()
 *
 * Description : Writes a block of data to the EEPROM over I2C. This function handles EEPROM
 *               page boundaries by splitting the data into chunks that fit within EEPROM page size.
 *               For each chunk, it constructs an I2C command to write the address followed by the
 *               data and waits for the write cycle to complete before proceeding to the next chunk.
 *
 * Parameters  : eeprom_addr_u16 - Starting EEPROM memory address to write to (16-bit).
 *               data_u8         - Pointer to the data buffer that holds the bytes to write.
 *               len             - Total number of bytes to write to EEPROM.
 *
 * Returns     : ESP_OK on success, or an error code of type esp_err_t on failure.
 *
 */
esp_err_t eeprom_write(uint16_t u16ChipAddr, const uint8_t *pu8Data, uint64_t u16DataLen)
{
	esp_err_t eErrStat = ESP_OK;

	/* Handle page writes */
	for (uint64_t u64ByteAddr = 0; u64ByteAddr < u16DataLen;) 
	{
		/* Calculate current page boundary */
		uint16_t u16CurrentAddr = u16ChipAddr + u64ByteAddr;
		uint16_t u16PgBndry = (u16CurrentAddr / EEPROM_PAGE_SIZE + 1) * EEPROM_PAGE_SIZE;
		uint16_t u16ByteToWrite = u16PgBndry - u16CurrentAddr;

		/* Limit bytes to write to what's left in our data */
		if (u16ByteToWrite > (u16DataLen - u64ByteAddr)) 
		{
			u16ByteToWrite = u16DataLen - u64ByteAddr;
		}

		/* Write this page (or partial page) */
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

		/* Write high byte of address */
		i2c_master_write_byte(cmd, (u16CurrentAddr >> 8) & 0xFF, true);

		/* Write low byte of address */
		i2c_master_write_byte(cmd, u16CurrentAddr & 0xFF, true);

		/* Write data for this page */
		i2c_master_write(cmd, (uint8_t *)pu8Data + u64ByteAddr, u16ByteToWrite, true);

		i2c_master_stop(cmd);
		eErrStat = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);

		if (eErrStat != ESP_OK) 
		{
			ESP_LOGE(EEPROM_TAG, "EEPROM write block failed at addr 0x%04X: %s", 
					u16CurrentAddr, esp_err_to_name(eErrStat));

			return eErrStat;
		}

		/* Wait for write cycle to complete */
		vTaskDelay(50 / portTICK_PERIOD_MS);

		/* Update our position */
		u64ByteAddr += u16ByteToWrite;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);

	return ESP_OK;
}


/*
 * Function     : eeprom_read()
 *
 * Description  : Reads a block of data from the EEPROM starting at the specified memory address.
 *                The function first writes the target address to the EEPROM using I2C protocol,
 *                then restarts the communication to read the specified number of bytes into the
 *                provided data buffer.
 *
 * Parameters   : u16ChipAddr - 16-bit EEPROM memory address to start reading from
 *                pu8Data     - Pointer to the buffer where the read data will be stored
 *                u16DataLen  - Number of bytes to read from EEPROM
 *
 * Returns      : esp_err_t
 *                - ESP_OK on successful read
 *                - Appropriate ESP error code if the operation fails
 *
 */
esp_err_t eeprom_read(uint16_t u16ChipAddr, uint8_t *pu8Data, uint64_t u16DataLen)
{
	esp_err_t eErrStat;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	/* First set address pointer */
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	/* Write high byte of address */
	i2c_master_write_byte(cmd, (u16ChipAddr >> 8) & 0xFF, true);

	/* Write low byte of address */
	i2c_master_write_byte(cmd, u16ChipAddr & 0xFF, true);

	/* Restart for reading */
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);

	/* Read all requested bytes */
	if (u16DataLen > 1) 
	{
		i2c_master_read(cmd, pu8Data, u16DataLen - 1, I2C_MASTER_ACK);
	}
	
	i2c_master_read_byte(cmd, pu8Data + u16DataLen - 1, I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	eErrStat = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (eErrStat != ESP_OK) 
	{
		ESP_LOGE(EEPROM_TAG, "EEPROM read block failed at addr 0x%04X: %s", u16ChipAddr, 
				esp_err_to_name(eErrStat));
	}

	return eErrStat;
}

/*
 * Function     : eeprom_read_byte()
 *
 * Description  : Reads a single byte from the specified EEPROM memory address. The function sends the
 *                memory address to the EEPROM and initiates a read operation over I2C. If the read is 
 *                successful, the read byte is returned. Otherwise, it returns 0xFF to indicate failure.
 *
 * Parameters   : u16Memaddr - 16-bit memory address in EEPROM to read from
 *
 * Returns      : uint8_t - Data byte read from the EEPROM memory address
 *                         - Returns 0xFF if the read operation fails
 *
 */
uint8_t eeprom_read_byte(uint16_t u16Memaddr) 
{
    uint8_t u8ReadByte = 0;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, (u16Memaddr >> 8) & 0xFF, true);
    i2c_master_write_byte(cmd, u16Memaddr & 0xFF, true);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &u8ReadByte, I2C_MASTER_NACK);

    i2c_master_stop(cmd);
    esp_err_t eErrStat = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    return (eErrStat == ESP_OK) ? u8ReadByte : 0xFF; /* Return 0xFF if read fails */
}

/*
 * Function     : detect_eeprom()
 *
 * Description  : This function attempts to detect the presence of an EEPROM device on the I2C bus
 *                by initiating a write operation to its I2C address. It sends a start condition,
 *                the device address in write mode, and a stop condition. If the device acknowledges
 *                the address, it confirms that the EEPROM is connected and responsive.
 *
 * Parameters   : None
 *
 * Returns      : true  - If the EEPROM is successfully detected on the I2C bus
 *                false - If the EEPROM is not detected or the communication fails
 *
 */
bool detect_eeprom(void)
{
	printf("Scanning I2C bus...\n");

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	i2c_master_stop(cmd);

	esp_err_t eErrStat = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (eErrStat != ESP_OK) 
	{
		ESP_LOGE(EEPROM_TAG, "EEPROM not detected: %s", esp_err_to_name(eErrStat));
		return false;
	}
	else
	{
		 printf("I2C device found at address: 0x%02X\n", EEPROM_I2C_ADDRESS);
	}

	printf("I2C scan complete.\n");

	return true;
}

/*
 * Function     : eeprom_erase()
 *
 * Description  : This function erases a single byte in the EEPROM at the specified chip address.
 *                It achieves this by writing 0xFF (commonly used as the erased value) to the given
 *                memory location. Internally, it utilizes the eeprom_write() function to perform the write.
 *
 * Parameters   : u16ChipAddr - EEPROM memory address to erase (uint16_t)
 *
 * Returns      : esp_err_t - ESP_OK on success or appropriate error code on failure
 *
 */
esp_err_t eeprom_erase(uint16_t u16ChipAddr) 
{
	uint8_t u8EraseByte = 0xFF;
	return eeprom_write(u16ChipAddr, &u8EraseByte, 1);
}


/*
 * Function     : uninit_i2c()
 *
 * Description  : This function deletes the I2C driver instance for I2C_NUM_0,
 *                effectively releasing the I2C resources. It checks the return
 *                status and prints a message indicating success or failure.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 */
static void uninit_i2c()
{
    esp_err_t eErrStat = i2c_driver_delete(I2C_NUM_0);
    if (eErrStat == ESP_OK)
    {
        printf("I2C uninitialized successfully!\n");
    }
    else
    {
        printf("Failed to uninitialize I2C: %s\n", esp_err_to_name(eErrStat));
    }
}

/*
 * Function     : erase_eeprom_chip()
 *
 * Description  : This function erases the entire EEPROM chip by writing 0xFF 
 *                to all memory locations. It writes in fixed-size chunks defined 
 *                by EEPROM_PAGE_SIZE, starting from EEPROM_START_ADDR up to 
 *                EEPROM_TOTAL_SIZE. This is useful for resetting or clearing 
 *                EEPROM data.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 */
void erase_eeprom_chip(void)
{
    uint8_t au8FillData[EEPROM_PAGE_SIZE];
    uint16_t u16Addr = EEPROM_START_ADDR;
    
   /* Fill the buffer with 0xFF (or 0x00 depending on your erase policy) */
   memset(au8FillData, 0xFF, EEPROM_PAGE_SIZE);
   
   ESP_LOGI(EEPROM_TAG, "Starting EEPROM chip erase...");
    while (u16Addr < EEPROM_TOTAL_SIZE) {
        eeprom_write(u16Addr, au8FillData, EEPROM_PAGE_SIZE);
        ESP_LOGI(EEPROM_TAG, "Erased page at address 0x%04X", u16Addr);
        u16Addr += EEPROM_PAGE_SIZE;
    }
    ESP_LOGI(EEPROM_TAG, "EEPROM chip erase completed successfully.");
}







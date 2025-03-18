#include "eeprom.h"

/* I2C initialization */
esp_err_t i2c_init(void)
{
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = I2C_MASTER_SDA_IO,
		.scl_io_num = I2C_MASTER_SCL_IO,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = I2C_MASTER_FREQ_HZ,
	};

	esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
	if(err != ESP_OK)
	{
		ESP_LOGE(EEPROM_TAG, "I2C parameter configuration failed: %s", esp_err_to_name(err));
		return err;
	}

	err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	if(err != ESP_OK) 
	{
		ESP_LOGE(EEPROM_TAG, "I2C driver installation failed: %s", esp_err_to_name(err));
		return err;
	}

	ESP_LOGI(EEPROM_TAG, "I2C initialized successfully");
	return ESP_OK;
}

/* Write a block of data to EEPROM */
esp_err_t eeprom_write(uint16_t eeaddress, const uint8_t *data, size_t len)
{
	esp_err_t ret = ESP_OK;

	/* Handle page writes */
	for (size_t written = 0; written < len; ) 
	{
		/* Calculate current page boundary */
		uint16_t current_addr = eeaddress + written;
		uint16_t page_boundary = (current_addr / EEPROM_PAGE_SIZE + 1) * EEPROM_PAGE_SIZE;
		uint16_t bytes_to_write = page_boundary - current_addr;

		/* Limit bytes to write to what's left in our data */
		if (bytes_to_write > (len - written)) 
		{
			bytes_to_write = len - written;
		}

		/* Write this page (or partial page) */
		i2c_cmd_handle_t cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

		/* Write high byte of address */
		i2c_master_write_byte(cmd, (current_addr >> 8) & 0xFF, true);

		/* Write low byte of address */
		i2c_master_write_byte(cmd, current_addr & 0xFF, true);

		/* Write data for this page */
		i2c_master_write(cmd, (uint8_t *)data + written, bytes_to_write, true);

		i2c_master_stop(cmd);
		ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);

		if (ret != ESP_OK) 
		{
			ESP_LOGE(EEPROM_TAG, "EEPROM write block failed at addr 0x%04X: %s", current_addr, esp_err_to_name(ret));
			return ret;
		}

		/* Wait for write cycle to complete */
		vTaskDelay(10 / portTICK_PERIOD_MS);

		/* Update our position */
		written += bytes_to_write;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);

	return ESP_OK;
}

/* Read a block of data from EEPROM */
esp_err_t eeprom_read(uint16_t eeaddress, uint8_t *data, size_t len)
{
	esp_err_t ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	/* First set address pointer */
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);

	/* Write high byte of address */
	i2c_master_write_byte(cmd, (eeaddress >> 8) & 0xFF, true);

	/* Write low byte of address */
	i2c_master_write_byte(cmd, eeaddress & 0xFF, true);

	/* Restart for reading */
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);

	/* Read all requested bytes */
	if (len > 1) 
	{
		i2c_master_read(cmd, data, len - 1, I2C_MASTER_ACK);
	}
	
	i2c_master_read_byte(cmd, data + len - 1, I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (ret != ESP_OK) 
	{
		ESP_LOGE(EEPROM_TAG, "EEPROM read block failed at addr 0x%04X: %s", eeaddress, esp_err_to_name(ret));
	}

	return ret;
}

/* Check if EEPROM is present */
bool eeprom_check_presence(void)
{
	printf("Scanning I2C bus...\n");

    //for (uint8_t addr = 0x03; addr <= 0x77; addr++)
    //{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (EEPROM_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
	//i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_stop(cmd);

	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	if (ret != ESP_OK) 
	{
		ESP_LOGE(EEPROM_TAG, "EEPROM not detected: %s", esp_err_to_name(ret));
		return false;
	}
	else
		 printf("I2C device found at address: 0x%02X\n", EEPROM_I2C_ADDRESS);
   // }
	printf("I2C scan complete.\n");
	//ESP_LOGI(EEPROM_TAG, "EEPROM detected successfully");
	return true;
}
esp_err_t eeprom_erase(uint16_t eeaddress) 
{
	uint8_t erase_byte = 0xFF;
	return eeprom_write(eeaddress, &erase_byte, 1);
}

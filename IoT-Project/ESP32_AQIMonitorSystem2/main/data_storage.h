#ifndef DATA_STORAGE_H
#define DATA_STORAGE_H

#include <stdint.h>
#include <stddef.h>

#define SPI_TAG			"SPI"
#define GPIO_MOSI       	23
#define GPIO_MISO       	19
#define GPIO_SCLK       	18
#define GPIO_CS         	5
#define CMD_SECTOR_ERASE   	0x20  /* SPI Flash Sector Erase command */
#define CMD_READ_DATA   	0x03
#define WRITE_CMD   		0x02  /* SPI Flash Write Command */
#define READ_STATUS_REG_CMD   	0x05  /* Command to read SPI Flash status register */
#define WIP_BIT_MASK          	0x01  /* Write-In-Progress bit mask */
#define WRITE_ENABLE_CMD  	0x06  /* Command to enable write operations on flash */
#define COUNTER_INITIAL_VALUE 	16
#define CLOUD_COUNTER_INITIAL_VALUE 16
#define MAX_ADDRESS (16 * 256 * 256)

void SPI_Receive(uint16_t u16Cmd, size_t u16CmdLen, uint64_t u64Addr, uint32_t u32AddrLen, uint8_t *pu8Buffer, uint32_t u32BufLen);

void sector_erase(uint8_t u8NumCector);

void spi_transmit(uint8_t *pu8Data, uint32_t u32Bytes);

void write_data (uint8_t u8PageNum, uint8_t *pu8DataToWrite, uint32_t u32Bytes);

void dataStorageTask(void *pvParameters);


#endif /* DATA_STORAGE_H */

#ifndef EEPROM_H
#define EEPROM_H

#pragma once
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_err.h"


/* I2C Configuration */
#define I2C_MASTER_NUM                  I2C_NUM_0
#define I2C_MASTER_SCL_IO               22
#define I2C_MASTER_SDA_IO               21
#define I2C_MASTER_FREQ_HZ              100000
#define I2C_MASTER_TX_BUF_DISABLE       0
#define I2C_MASTER_RX_BUF_DISABLE       0
#define I2C_MASTER_TIMEOUT_MS       	1000

/* EEPROM Configuration */
#define EEPROM_I2C_ADDRESS              0x50

/* EEPROM Address Mapping */
#define FACTORY_MODE_FLAG_ADDR          0x00   /* 1 byte */
#define EEPROM_MODE_FLAG_ADDR           0x01   /* 1 byte */
#define EEPROM_WIFI_SSID_ADDR           0x02   /* 32 bytes */
#define EEPROM_WIFI_PASS_ADDR           0x22   /* 64 bytes */
#define EEPROM_DEVICE_NAME_ADDR         0x62   /* 32 bytes */
#define EEPROM_LOCATION_ADDR            0x82   /* 32 bytes */
#define EEPROM_DEVICE_INFO_ADDR         0xA2   /* 64 bytes */

/* Versioning Information */
#define HW_VERSION_ADDR                 0x162  /* 20 bytes */
#define SW_VERSION_ADDR                 0x176  /* 20 bytes */
#define FW_VERSION_ADDR                 0x18A  /* 20 bytes */
#define MAX_VERSION_SIZE                20


/* AWS IoT Credentials */
#define EEPROM_AWS_CERT_ADDR            0x19E  				/* Starting address for the certificate */
#define EEPROM_AWS_CERT_SIZE_ADDR       (EEPROM_AWS_CERT_ADDR - 0x04) 	/* Address for storing certificate size */
#define EEPROM_AWS_KEY_ADDR             (EEPROM_AWS_CERT_ADDR + 0x6A4)  /* 1700 bytes gap for private key */
#define EEPROM_AWS_KEY_SIZE_ADDR        (EEPROM_AWS_KEY_ADDR - 0x04) 	/* Address for storing private key size */
#define EEPROM_AWS_ENDPOINT_ADDR        (EEPROM_AWS_KEY_ADDR + 0x6A4) 	/* 1700 bytes gap for endpoint */
#define EEPROM_AWS_ENDPOINT_SIZE_ADDR   (EEPROM_AWS_ENDPOINT_ADDR - 0x04) /* Address for storing endpoint size */
#define EEPROM_AWS_THING_NAME_ADDR      0x61E  				/* 64 bytes */
#define EEPROM_AWS_POLICY_NAME_ADDR     0x65E  				/* 64 bytes */
#define EEPROM_START_ADDR  		0x00           			/* Starting address of EEPROM */
#define EEPROM_TOTAL_SIZE  		32768          			/* Total size of EEPROM in bytes (e.g., 32KB) */
#define EEPROM_PAGE_SIZE   		64             			/* EEPROM page size in bytes */


/* Debugging */
#define EEPROM_TAG "EEPROM_DRIVER"

extern bool wifi_configured;

esp_err_t i2c_master_init(void);
esp_err_t eeprom_write(uint16_t u16ChipAddr, const uint8_t *pu8Data, uint64_t u16DataLen);
esp_err_t eeprom_read(uint16_t u16ChipAddr, uint8_t *pu8Data, uint64_t u16DataLen);
bool detect_eeprom(void);
esp_err_t eeprom_erase(uint16_t u16ChipAddr);
uint8_t eeprom_read_byte(uint16_t u16Memaddr);
void erase_eeprom_chip(void);






#endif /* EEPROM_H */

 /****************************************************************************
*  \file       i2c-eeprom-read.c
*
*  \details    Sample program to read the first 4 bytes of the
*              AT24C256 256 KiB EEPROM from the BeagleBone Black
*
*  \author     Fidility Solutions
*
*  \Reference  Chris Simmonds (chris@2net.co.uk) 
* *******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

/* Address of the EEPROM on the BeagleBone Black board */
#define I2C_ADDRESS 0x50

int main(void)
{
	int f;
	int n;
	char buf[10];
	uint16_t dataAddr = 0x0000;
	
	
	printf("**********************************\n");
	printf("****I2C EEPROM Test Program ******\n");
	printf("**********************************\n");

	/* Open the adapter and set the address of the I2C device */
	f = open("/dev/i2c-2", O_RDWR);
	if (f < 0) {
		perror("/dev/i2c-2:");
		return 1;
	}

	/* Set the address of the i2c slave device */
	if (ioctl(f, I2C_SLAVE, I2C_ADDRESS) == -1) {
		perror("ioctl I2C_SLAVE");
		return 1;
	}

	/* Set the 16-bit address to read (0) */
	buf[0] = dataAddr >> 8;		/* address byte 1 */
	buf[1] = dataAddr & 0xff;	/* address byte 2 */
	n = write(f, buf, 2);
	
	if (n == -1) {
		perror("write");
		return 1;
	}

	/* Now read 4 bytes from that address */
	n = read(f, buf, 4);
	if (n == -1) {
		perror("read");
		return 1;
	}
	printf("Reading first 4-bytes of data \n");
	printf("0x%x 0x%x 0x%x 0x%x\n",
		 buf[0], buf[1], buf[2], buf[3]);

	close(f);
	return 0;
}

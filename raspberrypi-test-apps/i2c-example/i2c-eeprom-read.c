 /****************************************************************************
* File      	: i2c-eeprom-read.c
*
* Description 	: Sample program to read the first 4 bytes of the
*            	  AT24C256 256 KiB EEPROM from the Raspberry pi
*
* Author    	: Fidility Solutions
*
* Reference  	: Chris Simmonds (chris@2net.co.uk) 
* *******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

/* Address of the EEPROM on the BeagleBone Black board */
#define I2C_ADDRESS 	0x50
#define BUF_SIZE 	50

int main(void)
{
        int FileDesc;
        int NumofBytes;
        char Buffer[BUF_SIZE];
        uint16_t dataAddr = 0x0000;
        
        
        printf("**********************************\n");
        printf("****I2C EEPROM Test Program ******\n");
        printf("**********************************\n");
	printf("This program demonistartes reading first 4 bytes of data from I2C EEPROM\n");

        /* Open the adapter and set the address of the I2C device */
        FileDesc = open("/dev/i2c-1", O_RDWR);
        if (FileDesc < 0) {
                perror("device file failed to open");
                return 1;
        }

        /* Set the address of the i2c slave device */
        if (ioctl(FileDesc, I2C_SLAVE, I2C_ADDRESS) == -1) {
                perror("ioctl I2C_SLAVE");
                return 1;
        }

        /* Set the 16-bit address to read (0) */
        Buffer[0] = dataAddr >> 8;              /* address byte 1 */
        Buffer[1] = dataAddr & 0xff;    	/* address byte 2 */
        NumofBytes = write(FileDesc,Buffer, 2);
        
        if (NumofBytes == -1) {
                perror("write fail");
                return 1;
        }

        /* Now read 4 bytes from that address */
        NumofBytes = read(FileDesc, Buffer, 4);
        if (NumofBytes == -1) {
                perror("read fail");
                return 1;
        }
        printf("\nReading first 4-bytes of data \n");
        printf("Data:0x%x 0x%x 0x%x 0x%x\n",
                 Buffer[0], Buffer[1],Buffer[2], Buffer[3]);

        close(FileDesc);
        return 0;
}


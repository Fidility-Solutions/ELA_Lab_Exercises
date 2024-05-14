 /****************************************************************************
* file 		: i2c-eeprom-read-write.c
*
* Description	: Sample program to write and read the first 64 bytes of the
*                 AT24C256 256 KiB EEPROM from BeagleBone Black
*
* Author      	: Fidility Solutions
*
* Reference  	: Chris Simmonds (chris@2net.co.uk) 
* *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <linux/i2c.h>

#define BOARD 1

#if BOARD == 1
    #define DEVICE_PATH "/dev/i2c-1"
#elif BOARD == 2
    #define DEVICE_PATH "/dev/i2c-2"
#else
    #error "Unsupported BOARD value"
#endif

#define PAGE_SIZE 64

#define DEVICE_ADDR 0x50 	/* 0b1010xxxx */


int file_desc;
char buffer[PAGE_SIZE + 2]; 	/* 64 bytes + 2 for the address */

void teardownI2C()
{
    if(close(file_desc) == -1){
	     perror("Error closing file descriptor");
    }
}



/* 
 * Function: setupI2C
 *  
 * Description: Initializes communication via the I2C (Inter-Integrated Circuit) protocol.
 *              This function opens a file descriptor for communication with an I2C device
 *              specified by DEVICE_PATH. It then sets the I2C slave address using the
 *              ioctl system call. If either of these operations fails, the function prints
 *              an error message and exits the program with a status code indicating an error.
 *  
 *  Returns: void (no return value)
 */


void setupI2C()
{
	/* Attempt to open a file descriptor for communication with an I2C device */
   	file_desc = open(DEVICE_PATH, O_RDWR);
   	if(file_desc < 0)
   	{
   		printf("%s\n", strerror(errno));
   		exit(1);
   	}
   	 /* Set the I2C slave address for communication */
   	if(ioctl(file_desc, I2C_SLAVE, DEVICE_ADDR) < 0)
   	{
   		printf("%s\n", strerror(errno));
		/* Perform cleanup */
   		teardownI2C();
   		exit(1);
   	}
}

/*
 * Function: 	write_to_device
 *  
 * Description: Writes data to a device via the I2C (Inter-Integrated Circuit) protocol.
 *              This function constructs an I2C message containing the device address, 
 *              register address, and data to be written. It then sends the message to
 *              the device using the ioctl system call. If the operation fails, an error
 *              message is printed, and the function returns -1. Otherwise, it returns 0
 *              indicating success.
 *  
 * Parameters:
 *   		- addr_hi: The high byte of the register address to write to.
 *   		- addr_lo: The low byte of the register address to write to.
 *   		- buf	 : A pointer to the buffer containing the data to write.
 *   		- len	 : The number of bytes to write.
 *  
 * Returns:
 *   		- 0 on success.
 *   		- -1 on failure.
 */

int write_to_device(char addr_hi, char addr_lo, char * buf, int len)
{
     	struct i2c_rdwr_ioctl_data msg_rdwr;
    	struct i2c_msg i2cmsg;
    	char my_buf[PAGE_SIZE + 2];
	/* Check if the length of data to write exceeds the maximum allowed */
    	if(len > (PAGE_SIZE + 2))
    	{
    		printf("Can't write more than %d bytes at a time.\n", PAGE_SIZE);
    		return -1;
    	}
    	
    	int i;
	/* Populate the first two bytes of the buffer with the register address */
    	my_buf[0] = addr_hi;
    	my_buf[1] = addr_lo;
	/* Copy the data to be written into the buffer */
    	for(i= 0; i < len; i++)
    	{
    		my_buf[2+i] = buf[i];
    	}
    	/* Configure the I2C message */
    	msg_rdwr.msgs = &i2cmsg;
    	msg_rdwr.nmsgs = 1;
    	i2cmsg.addr  = DEVICE_ADDR;
    	i2cmsg.flags = 0;
    	i2cmsg.len   =2 + len;
    	i2cmsg.buf   = (__u8 *)my_buf;
	/* Send the I2C message to the device */
    	errno=ioctl(file_desc,I2C_RDWR,&msg_rdwr);
    	if(errno < 0)
    	{
    		printf("write_to_device(): %s\n", strerror(errno));
    		return -1;
    	} 
    	return 0;

}

/*
 * Function: 	read_from_device
 *
 * Description: Reads data from a device via the I2C (Inter-Integrated Circuit) protocol.
 *              This function first resets the device address by writing a null message
 *              (address reset) to the device using the `write_to_device` function. Then,
 *              it constructs an I2C message for reading data from the device and sends
 *              it using the ioctl system call. If the operation fails, an error message
 *              is printed, and the function returns -1. Otherwise, it returns 0 indicating
 *              success.
 *
 * Parameters:
 *  		- addr_hi: The high byte of the register address to read from.
 *  		- addr_lo: The low byte of the register address to read from.
 *  		- buf	 : A pointer to the buffer where the read data will be stored.
 *  		- len	 : The number of bytes to read.
 *
 * Returns:
 *  - 0 on success.
 *  - -1 on failure.
*/
int read_from_device(char addr_hi, char addr_lo, char * buf, int len)
{
   	 struct i2c_rdwr_ioctl_data msg_rdwr;
   	 struct i2c_msg             i2cmsg;

	 /* write the address location to read */
   	 if(write_to_device(addr_hi, addr_lo ,NULL,0) < 0)
   	 {
   	 	printf("read_from_device(): address reset did not work\n");
   	 	return -1;
   	 }
   	 /* Configure the I2C message for reading data from the device */
   	 msg_rdwr.msgs = &i2cmsg;
   	 msg_rdwr.nmsgs = 1;

   	 i2cmsg.addr  = DEVICE_ADDR;
   	 i2cmsg.flags = I2C_M_RD; 	/* Read operation */
   	 i2cmsg.len   = len;		/* Number of bytes to read */
   	 i2cmsg.buf   = (__u8 *)buf;	/* Buffer to store the read data */

	 /* Send the I2C message to read data from the device */
   	 if(ioctl(file_desc,I2C_RDWR,&msg_rdwr)<0)
   	 {
   	 	printf("read_from_device(): %s\n", strerror(errno));
   	 	return -1;
   	 }
   	 return 0;
}
/*
 * Function	: fill_buffer
 * 
 * Description	: Fills a buffer with characters from another buffer or pads it with a default character.
 *                This function takes a pointer to a source buffer (`buf`) and copies characters from
 *                it to a destination buffer (`buffer`). It copies characters until either the source
 *                buffer is exhausted or the destination buffer (`buffer`) is filled (PAGE_SIZE bytes).
 *                If the source buffer contains fewer characters than PAGE_SIZE, the remaining space
 *                in the destination buffer is filled with a default character ('*').
 * 
 * Parameters	:
 *  		  - buf: A pointer to the source buffer containing characters to copy.
 * 
 * Returns	: void (no return value)
 */
void fill_buffer(char *buf)
{
    int i = 0;
     /* Copy characters from the source buffer to the destination buffer until either the source buffer is exhausted or the destination buffer is filled */
    while(i < PAGE_SIZE && *buf)
    {
    	buffer[i++] = *buf++;
    }
    /* If there is still space left in the destination buffer, fill it with a default character ('*') */
    while(i++ < (PAGE_SIZE-1))
    {
    	buffer[i++] = '*';
    }
}


/*
 * Function	: main
 * 
 * Description	: The main entry point of the program. This function demonstrates the read and write 
 *                operations on an I2C EEPROM. It first prints a message indicating the purpose of 
 *                the program. Then, it initializes I2C communication by setting up the device file 
 *                and slave address. Next, it fills a buffer with a message to be written to the 
 *                EEPROM and writes this data to the EEPROM at a specified location. After a short 
 *                delay, it creates a new buffer and reads data from the EEPROM into this buffer. 
 *                Finally, it prints the read data and cleans up the I2C setup before exiting.
 * 
 * Returns:
 *  - EXIT_SUCCESS (0) if the program executes successfully.
 */

int main(void)
{
	printf("This program will demonstrate I2C EEPROM Read and Write operation\n");
	/* This function will open device file and set the slave address for communication */
    	setupI2C();
	printf("I2C initialized\n");
	/* This function to fill the buffer to write on EEPROM */
    	fill_buffer("I2C device file sets the EEPROM's I2C address reads/write data");
	/* This function write the data in buffer to EEPROM  with specified location*/
    	write_to_device(0x01, 0x00, buffer,(PAGE_SIZE-2));

	printf("%ld bytes of data written to EEPROM :%s\n",sizeof(buffer)-2,buffer);
 	sleep(2);
    	char newbuf[PAGE_SIZE];
    	memset(newbuf,0x05,PAGE_SIZE);
	/* This function will read the data from EEPROM and print the data */
    	if(read_from_device(0x01, 0x00, newbuf, PAGE_SIZE)==0){
    		printf("%ld bytes of data read from EEPROM:%s\n",sizeof(newbuf), newbuf);
    	}
	
	/* This function clear the I2C setup */
    	teardownI2C();
    	return EXIT_SUCCESS;
}

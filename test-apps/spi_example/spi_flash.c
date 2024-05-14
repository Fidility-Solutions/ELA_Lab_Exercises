#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev0.0"
#define SPI_SPEED_HZ 1000000
#define BUFFER_SIZE 256

int main() {
    	int spi_fd;
   	 unsigned char tx_buffer[BUFFER_SIZE] = {0}; 
   	 unsigned char rx_buffer[BUFFER_SIZE] = {0}; 

   	 /* Open SPI device */
   	 spi_fd = open(SPI_DEVICE, O_RDWR);
   	 if(spi_fd < 0) {
   	     	perror("Error opening SPI device");
   	     	return 1;
   	 }

   	 /* Configure SPI parameters */
   	 int mode = SPI_MODE_0;
   	 if(ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1) {
   	     	perror("Error setting SPI mode");
   	    	 close(spi_fd);
   	     	return 1;
   	 }

   	 int bits = 8; // Bits per word
   	 if(ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
   	     	perror("Error setting SPI bits per word");
   	     	close(spi_fd);
   	     	return 1;
   	 }

   	 int speed = SPI_SPEED_HZ;
   	 if(ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
   	     	perror("Error setting SPI speed");
   	     	close(spi_fd);
   	     	return 1;
   	 }

   	 /* Write enable command */
   	 tx_buffer[0] = 0x06; /* Write enable command for W25Q64 */
   	 if(write(spi_fd, tx_buffer, 1) != 1) {
   	     	perror("Error writing write enable command");
   	     	close(spi_fd);
   	     	return 1;
   	 }

   	 /* Write command */
   	 tx_buffer[0] = 0x02; /* Write command for W25Q64 */
   	 tx_buffer[1] = 0x00; /* Address MSB (assuming you want to write to address 0x000000) */
   	 tx_buffer[2] = 0x00; /* Address mid */
   	 tx_buffer[3] = 0x00; /* Address LSB */
   	 tx_buffer[4] = 'R';  /* Data to be written */
	 tx_buffer[5] = 'K';
	 tx_buffer[6] = 'C';
	 tx_buffer[7] = 'H';
   	 if(write(spi_fd, tx_buffer, 8) != 8) {
   	     perror("Error writing to flash memory");
   	     close(spi_fd);
   	     return 1;
   	 }
	 sleep(2);

   	 /* Read command */
   	 tx_buffer[0] = 0x03; /* Read command for W25Q64 */
   	 tx_buffer[1] = 0x00; /* Address MSB (assuming you want to read from address 0x000000) */
   	 tx_buffer[2] = 0x00; /* Address mid */
   	 tx_buffer[3] = 0x00; /* Address LSB */
   	 if(write(spi_fd, tx_buffer, 4) != 4) {
   	     perror("Error writing read command");
   	     close(spi_fd);
   	     return 1;
   	 }
	 sleep(2);
   	 if(read(spi_fd, rx_buffer, BUFFER_SIZE) < 0) {
   	     	perror("Error reading from flash memory");
   	     	close(spi_fd);
   	     	return 1;
   	 }

   	 /* Display read data */
	for(int i=0;i<9;i++){
		printf("written data:%c\n",tx_buffer[i+4]);
   	 	printf("Read data: %c\n", rx_buffer[i]);
	}

   	 /* Close SPI device */
   	 close(spi_fd);

   	 return 0;
}


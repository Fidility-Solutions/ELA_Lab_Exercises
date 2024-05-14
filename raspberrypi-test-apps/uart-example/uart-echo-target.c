
/*
 * File		: uart_echo_target.c
 * 
 * Description	: This program demonstrates UART communication on Linux using the ttyS0 serial port.
 *                It sends a message over UART1 and waits for a response from the loopback.
 *
 * Author	: Fidility Solutions.
 *
 * Date		: 
 *
 * Reference	: [Any relevant references]
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define UART_DEVICE "/dev/ttyS0"
#define BUFFER_SIZE 256

/*
 * Function	: main
 * ----------------------------
 * Description	: This function demonstrates UART communication by sending a message over UART1 and waiting for an echo back response.
 *                It opens the serial port, configures it, sends a message, waits for a response, and prints the received data.
 *                After completing the communication, it closes the serial port.
 *
 * returns	: 0 on success, 1 on error
 */

int main(void) {
	printf("This program is designed to showcase UART echo functionality, where data sent over the UART interface (ttyS0 or UART1) is echoed back.\n");
   	int UartFileDisc;
   	struct termios options;
   	char TxBuffer[BUFFER_SIZE] = "This is sample test code for data tx & rx over UART1 loopback\n";
   	char RxBuffer[BUFFER_SIZE];

   	 /* Open the serial port */
   	UartFileDisc = open(UART_DEVICE, O_RDWR | O_NOCTTY);
   	 if (UartFileDisc == -1) {
   	     	perror("Error opening serial port");
   	     	return 1;
   	 }
   	 printf("The ttyS0(UART1) Device file opened successfully\n");

   	 /* Get the current serial port options */
   	 if(tcgetattr(UartFileDisc, &options) != 0) {
   	     	perror("Error getting serial port options");
   	     	close(UartFileDisc);
   	     	return 1;
   	 }

   	 /* Set the baud rate to 9600 */
   	 cfsetispeed(&options, B9600);
   	 cfsetospeed(&options, B9600);

   	 /* Set other serial port options */
   	 options.c_cflag &= ~PARENB;     		/* Disable parity */
   	 options.c_cflag &= ~CSTOPB;     		/* Use one stop bit */
   	 options.c_cflag &= ~CSIZE;      		/* Clear the character size bits */
   	 options.c_cflag |= CS8;         		/* 8 data bits */
   	 options.c_cflag &= ~CRTSCTS;    		/* Disable hardware flow control */
   	 options.c_cflag |= CREAD | CLOCAL; 		/* Enable receiver, ignore modem control lines */
   	 options.c_iflag &= ~(IXON | IXOFF | IXANY); 	/* Disable software flow control */
   	 options.c_oflag &= ~OPOST;      		/* Raw output mode */
   	 options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 	/* Disable canonical mode and echoing */
   	 options.c_cc[VTIME] = 10;          		/* Wait for up to 2s (20 deciseconds), returning as soon as any data is received. */
   	 options.c_cc[VMIN] = 250;


   	 /* Set the new serial port options */
   	 if(tcsetattr(UartFileDisc, TCSANOW, &options) != 0) {
   	     	perror("Error setting serial port options");
   	     	close(UartFileDisc);
   	     	return 1;
   	 }
	 printf("The new serial port options are set to ttyS0(UART1)\n");
   	 ssize_t SentBytes = write(UartFileDisc,TxBuffer, strlen(TxBuffer));
   	 if(SentBytes < 0) {
   	     	perror("Error writing to serial port");
   	     	close(UartFileDisc);
   	     	return 1;
   	 } 
	 else if (SentBytes != strlen(TxBuffer)) {
   	     	fprintf(stderr, "Error: Could not write all data to serial port\n");
   	     	close(UartFileDisc);
   	     	return 1;
   	 }
   	 printf("%ld bytes of data sent successfully over ttyS0(UART1):%s\n",SentBytes,TxBuffer);

   	 /* Read data from the serial port */
   	 printf("Waiting to read data on ttyS0(UART1)\n");

   	  ssize_t RecvBytes = read(UartFileDisc, RxBuffer,BUFFER_SIZE);
   	 if(RecvBytes <0) {
   	     	perror("Error reading from serial port");
   	     	close(UartFileDisc);
   	     	return 1;
   	 }
	else if(RecvBytes > 0) {
        	/* Null-terminate the received data */
        	RxBuffer[RecvBytes] = '\0';
		printf("%ld bytes of data received successfully on over ttyS0(UART1):%s\n",RecvBytes,RxBuffer);
    	} 
	else {
        	printf("No data received from serial port\n");
    	}

    	/* Close the serial port */
    	close(UartFileDisc);

    	return 0;
}


/*
 * File		: uart-transmit-host.c
 *
 * Description	: This program demonstrates UART communication between a host (Ubuntu) and a target device over ttyUSB0.
 *                It configures the serial port, sends a message from the host to the target, and waits for a response.
 *
 * Author	: Fidility Solutions.
 *
 * Reference	: Linux Programming Interface.
 */

/* C library headers */
#include <stdio.h>
#include <string.h>

/* Linux headers */
#include <fcntl.h> 	/* Contains file controls like O_RDWR */
#include <errno.h> 	/* Error integer and strerror() function */
#include <termios.h> 	/* Contains POSIX terminal control definitions */
#include <unistd.h> 	/* write(), read(), close() */
#define BUF_SIZE 64


/*
 * Function	: main
 * ----------------------------
 * Description	: This function demonstrates UART communication between a host (Ubuntu) and a target device over ttyUSB0.
 *                It configures the serial port, sends a message from the host to the target, and waits for a response.
 *
 * returns	: 0 on success, 1 on error
 */


int main(void) {
	printf("This program will demonistrate UART Tx & Rx over the ttyUSB0 from host to target and vice cersa\n");
  	/* Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device) */
  	int SerialPort = open("/dev/ttyUSB0", O_RDWR);
  	if(SerialPort < 0)
  	{
  	        printf("ttyUSB0 device file failed to open\n");
  	        return 1;
  	}
	printf("ttyUSB0 device file opened succesfully\n");
  	/* Create new termios struct, we call it 'tty' for convention */
  	struct termios tty;

  	/* Read in existing settings, and handle any error */
  	if(tcgetattr(SerialPort, &tty) != 0) {
  	    printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
  	    return 1;
  	}

  	tty.c_cflag &= ~PARENB; 	/* Clear parity bit, disabling parity (most common) */
  	tty.c_cflag &= ~CSTOPB; 	/* Clear stop field, only one stop bit used in communication (most common) */
  	tty.c_cflag &= ~CSIZE; 		/* Clear all bits that set the data size  */
  	tty.c_cflag |= CS8; 		/* 8 bits per byte (most common) */
  	tty.c_cflag &= ~CRTSCTS; 	/* Disable RTS/CTS hardware flow control (most common) */
  	tty.c_cflag |= CREAD | CLOCAL; 	/* Turn on READ & ignore ctrl lines (CLOCAL = 1) */

  	tty.c_lflag &= ~ICANON;
  	tty.c_lflag &= ~ECHO; 		/* Disable echo */
  	tty.c_lflag &= ~ECHOE; 		/* Disable erasure */
  	tty.c_lflag &= ~ECHONL; 	/* Disable new-line echo */
  	tty.c_lflag &= ~ISIG; 		/* Disable interpretation of INTR, QUIT and SUSP */
  	tty.c_iflag &= ~(IXON | IXOFF | IXANY); 				/* Turn off s/w flow ctrl */
  	tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); 	/* Disable any special handling of received bytes */

  	tty.c_oflag &= ~OPOST; 		/* Prevent special interpretation of output bytes (e.g. newline chars) */
  	tty.c_oflag &= ~ONLCR; 		/* Prevent conversion of newline to carriage return/line feed */
  	tty.c_oflag &= ~XTABS; 		/* Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX) */
  	tty.c_oflag &= ~ENOENT; 	/* Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX) */

  	tty.c_cc[VTIME] = 200;    	/* Wait for up to 2s (20 deciseconds), returning as soon as any data is received. */
  	tty.c_cc[VMIN] = 10;

  	/* Set in/out baud rate to be 9600 */
  	cfsetispeed(&tty, B9600);
  	cfsetospeed(&tty, B9600);

  	/* Save tty settings, also checking for error */
  	printf("The parameters are set successfully for ttyUSB0 \n");
  	if (tcsetattr(SerialPort, TCSANOW, &tty) != 0) {
  	    	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
  	    	return 1;
  	}

  	/* Write to ttyUSB0 port*/
  	char WriteBuff[BUF_SIZE] =  "Hello, Raspberry pi target! this is ubuntu host\n";
 	int WriteBytes = write(SerialPort,WriteBuff,BUF_SIZE);
	if(WriteBytes > 0){
	 	printf("Data  with %d bytes sent successfully from host over ttyUSB0:%s\n",WriteBytes,WriteBuff);
 	}
 	else{
	 	printf("Failed to sent data over ttyUSB0\n");
	 	return 1;
 	}

  	/* Allocate memory for receive buffer, set size according to your needs */
  	char RecvBuff[BUF_SIZE]={0};
	
  	int RecvBytes=0;
  	/* Receive bytes. The behaviour of read() (e.g. does it block?, how long does it block for?) depends on the configuration
        settings above, specifically VMIN and VTIM */
 	 printf("\nWaiting for response from target\n");
 	 RecvBytes = read(SerialPort, &RecvBuff, BUF_SIZE);
 	 printf("%d bytes of data received from target: %s\n",RecvBytes, RecvBuff);
 	 /* RecvBytes is the number of bytes recived. RecvBytes may be 0 if no bytes were received, and can also be -1 to signal an error. */
 	 if(RecvBytes < 0) {
 	     printf("Error reading: %s", strerror(errno));
 	     return 1;
 	 }
 	
 	 
	 close(SerialPort);
	 printf("The device file closed successfully\n");
 	 return 0; /* success */


}


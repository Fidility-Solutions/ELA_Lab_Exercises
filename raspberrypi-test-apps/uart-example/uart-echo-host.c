/*
* File Name	: uart-echo-host.c
*
* Description	: This program demonstrates basic UART communication using on host.
*
* Author	: fidility solutions.
*
* Reference	: Linux Programming Interface.
*
*/

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h> /* using the termios.h library */

/*
* Function	: main
*
* Description	: The main function initializes UART communication, reads data from the UART, and writes data to the UART on Host.
*
* Parameters	: None
*
* Return	: int - Returns 0 on successful execution, -1 if any operation fails.
*/

int main(void){
	int file, count;
	
	if ((file = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY))<0){
		perror("UART: Failed to open the file.\n");
		return -1;
	}
	
	struct termios options; 			/* the termios structure is vital */
	tcgetattr(file, &options); 			/* sets the parameters associated with file */
	
	/* Set up the communications options: */
	/* 9600 baud, 8-bit, enable receiver, no modem control lines */
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL; 		/* ignore partity errors, CR -> newline */
	tcflush(file, TCIFLUSH); 			/* discard file information not transmitted */
	tcsetattr(file, TCSANOW, &options); 		/* changes occur immmediately */
	
	unsigned char receive[100]; 			/* declare a buffer for receiving data */
	
	/* Receive the data */
	if ((count = read(file, (void*)receive, 100))<0){
		perror("Failed to read from the input\n");
		return -1;
	}
	if (count==0) 
		printf("There was no data available to read!\n");
	else 
	{
		printf("The following was read in [%d]: %s\n",count,receive);
	}
	/* give the  a time to respond */
	usleep(100000);
	
	/* The string to send */
	unsigned char transmit[18] = "Hi From Host!";
	/* Send the string */
	if ((count = write(file, &transmit,18))<0){ 
		perror("Failed to write to the output\n");
		return -1;
	}

	close(file);
	return 0;
}

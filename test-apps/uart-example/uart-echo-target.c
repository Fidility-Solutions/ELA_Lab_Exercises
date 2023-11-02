#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h> // using the termios.h library

int main(){
	int file, count;
	
	if ((file = open("/dev/ttyS4", O_RDWR | O_NOCTTY | O_NDELAY))<0){
		perror("UART: Failed to open the file.\n");
		return -1;
	}
	
	struct termios options; // the termios structure is vital
	tcgetattr(file, &options); // sets the parameters associated with file
	
	// Set up the communications options:
	// 9600 baud, 8-bit, enable receiver, no modem control lines
	options.c_cflag = B9600 | CS8 | CREAD | CLOCAL;
	options.c_iflag = IGNPAR | ICRNL; // ignore partity errors, CR -> newline
	tcflush(file, TCIFLUSH); // discard file information not transmitted
	tcsetattr(file, TCSANOW, &options); // changes occur immmediately
	
	unsigned char transmit[25] = "Hello From BeagleBone!"; // the string to send
	if ((count = write(file, &transmit,25))<0){ // send the string
		perror("Failed to write to the output\n");
		return -1;
	}
	
	usleep(100000); // give the Arduino a chance to respond
	
	unsigned char receive[100]; // declare a buffer for receiving data
	
	if ((count = read(file, (void*)receive, 100))<0){ // receive the data
		perror("Failed to read from the input\n");
		return -1;
	}
	if (count==0) 
		printf("There was no data available to read!\n");
	else 
	{
		printf("The following was read in [%d]: %s\n",count,receive);
	}
	
	close(file);
	return 0;
}

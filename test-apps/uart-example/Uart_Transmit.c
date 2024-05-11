// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

int main() {
  // Open the serial port. Change device path as needed (currently set to an standard 	FTDI USB-UART cable type device)
  
  int serial_port = open("/dev/ttyUSB1", O_RDWR);
  if(serial_port < 0)
  {
	  printf("Device failed to open\n");
	  return 1;
  }
printf("The device file opened\n");
  // Create new termios struct, we call it 'tty' for convention
  struct termios tty;

  // Read in existing settings, and handle any error
  if(tcgetattr(serial_port, &tty) != 0) {
      printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
  tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
  tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size 
  tty.c_cflag |= CS8; // 8 bits per byte (most common)
  tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
  tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

  tty.c_lflag &= ~ICANON;
  tty.c_lflag &= ~ECHO; // Disable echo
  tty.c_lflag &= ~ECHOE; // Disable erasure
  tty.c_lflag &= ~ECHONL; // Disable new-line echo
  tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
  tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
  tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

  tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
  tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
  // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
  // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

  tty.c_cc[VTIME] = 100;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
  tty.c_cc[VMIN] = 10;

  // Set in/out baud rate to be 9600
  cfsetispeed(&tty, B9600);
  cfsetospeed(&tty, B9600);

  // Save tty settings, also checking for error
  printf("The settings arraged for serial port\n");
  if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
      printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
      return 1;
  }

  // Write to serial port
  unsigned char msg[] = { 'H', 'e', 'l', 'l', 'o', '\r' };
 if( write(serial_port, msg, sizeof(msg)) > 0){
	 printf("Data written successful\n");
 }
 else{
	 printf("failed to write data to serial port\n");
	 return 1;
 }


  // Allocate memory for read buffer, set size according to your needs
  char read_buf [256];

  // Normally you wouldn't do this memset() call, but since we will just receive
  // ASCII data for this example, we'll set everything to 0 so we can
  // call printf() easily.
  memset(&read_buf, '\0', sizeof(read_buf));

  // Read bytes. The behaviour of read() (e.g. does it block?,
  // how long does it block for?) depends on the configuration
  // settings above, specifically VMIN and VTIME
  printf("Waiting for response from target\n");
  int num_bytes = read(serial_port, &read_buf, sizeof(read_buf));
  printf("Read %d bytes. Received message: %s", num_bytes, read_buf);
	printf("data read\n");
  // n is the number of bytes read. n may be 0 if no bytes were received, and can also be -1 to signal an error.
  if (num_bytes < 0) {
      printf("Error reading: %s", strerror(errno));
      return 1;
  }
 

  // Here we assume we received ASCII data, but you might be sending raw bytes (in that case, don't try and
  // print it to the screen like this!)
  printf("Read %i bytes. Received message: %s", num_bytes, read_buf);

  close(serial_port);
  return 0; // success


}

//	#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <string.h>
//#include <termios.h>
//#include <sys/wait.h>
//#define UART_DEVICE "/dev/ttyUSB1"  // UART device file
//
//int main() {
//    int uart_fd;
//    char tx_buffer[] = "Hello, UART1 !";
//
//    // Open UART device
//    uart_fd = open(UART_DEVICE, O_RDWR | O_NOCTTY);
//    if (uart_fd == -1) {
//        perror("Failed to open UART device");
//        exit(EXIT_FAILURE);
//    }
//
//    // Configure UART settings
//    struct termios options;
//    tcgetattr(uart_fd, &options);
//    cfsetispeed(&options, B9600);  // Set baud rate to 9600
//    cfsetospeed(&options, B9600);
//    options.c_cflag |= (CLOCAL | CREAD);  // Enable transmitter and set local mode
//    options.c_cflag &= ~PARENB;           // No parity
//    options.c_cflag &= ~CSTOPB;           // One stop bit
//    options.c_cflag &= ~CSIZE;
//    options.c_cflag |= CS8;               // 8 data bits
//    tcsetattr(uart_fd, TCSANOW, &options);
//
//    // Transmit data over UART
//    ssize_t num_bytes = write(uart_fd, tx_buffer, strlen(tx_buffer));
//    if (num_bytes == -1 && num_bytes == 0 ) {
//        perror("Failed to write to UART");
//        exit(EXIT_FAILURE);
//    } else {
//        printf("Sent data: %s\n", tx_buffer);
//    }
//
//    // Close UART device
//    close(uart_fd);
//
//    return 0;
//}
//

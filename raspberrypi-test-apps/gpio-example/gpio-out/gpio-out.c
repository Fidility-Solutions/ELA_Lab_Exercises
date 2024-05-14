/*******************************************************************************
 * File		: gpio-out.c
 *
 * Description	: This file contains a C program to control LEDs connected to GPIO pins on a Raspberry Pi using the sysfs interface.
 *
 * Author	: Fidility Solutions.
 *
 * Reference	: Embedded Linux Interface
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdint.h>

#define PIN_1 17	/* GPIO pin for LED 1 */
#define PIN_2 22	/* GPIO pin for LED 2 */
#define PIN_3 27	/* GPIO pin for LED 3 */
#define NUM_PINS 3	/* Total number of GPIO pins used */

/*
 * Function	: exportPin
 *
 * Description	: This function exports the specified GPIO pin by writing its pin number to the /sys/class/gpio/export file.
 *
 * Arguments	: GpioPin - The GPIO pin number to be exported.
 *
 * Returns	: None.
 */

void ExportGpioPin(int GpioPin) {
	/* Open export file */
    	FILE *ExportFile = fopen("/sys/class/gpio/export", "w");
    	if (ExportFile == NULL) {
        	perror("Error opening export file");
        	exit(EXIT_FAILURE);
    	}
	/* Write GPIO pin number to export file */
    	fprintf(ExportFile, "%d", GpioPin);
    	fclose(ExportFile);
}


/*
 * Function	: setDirection
 *
 * Description	: This function sets the direction of the specified GPIO pin (input or output) by writing to the /sys/class/gpio/gpio<PinNumber>/direction file.
 *
 * Arguments	: GpioPin  - The GPIO pin number.
 *               direction - The direction to set ("in" for input, "out" for output).
 *
 * Returns	: None.
 */
void SetPinDirection(int GpioPin, const char *direction) {
    	char GpioDirPath[100];
    	snprintf(GpioDirPath, sizeof(GpioDirPath), "/sys/class/gpio/gpio%d/direction", GpioPin);
	/* Open direction file */
    	FILE *DirectionFile = fopen(GpioDirPath, "w");
    	if(DirectionFile == NULL) {
        	perror("Error opening direction file");
        	exit(EXIT_FAILURE);
    	}
	/* Set direction */
    	fprintf(DirectionFile, "%s", direction);
    	fclose(DirectionFile);
}



/*
 * Function	: TurnOnLeds
 *
 * Description	: This function sequentially turns on the LEDs connected to all GPIO pins. It opens the file descriptor for each pin, writes "1" to turn the LED on,
 *             	  waits for a second, then writes "0" to turn it off, creating a blinking effect.
 *
 * Arguments	: None.
 *
 * Returns	: None.
 */
void TurnOnLeds(void) {
	uint8_t i;
	char GpioValPath[NUM_PINS][100];
	FILE *ValueFile[NUM_PINS];
	/* Open value files for all 3 GPIO pins */
	for(i = 0;i < NUM_PINS;i++){
		snprintf(GpioValPath[i], sizeof(GpioValPath[i]), "/sys/class/gpio/gpio%d/value",(i == 0 ? 17 : (i == 1 ? 22 : 27)));
		ValueFile[i] = fopen(GpioValPath[i], "w");
    		if(ValueFile[i] == NULL) {
        		perror("Error opening gpio file");
        		exit(EXIT_FAILURE);
    		}
	}
	printf("  Blinking each led\n");
	/* Blink each LED */
	for(i = 0;i < NUM_PINS;i++){
        	fprintf(ValueFile[i], "1");
		fflush(ValueFile[i]);
		sleep(1);
		fprintf(ValueFile[i],"0");
		fflush(ValueFile[i]);
		sleep(1);
	}
	/* turn on/off two LEDs */
	/* Simultaneously turn on all LEDs */
	printf("  Blinking 2leds\n");
//	for(i = 0;i < NUM_PINS;i++){
//		fprintf(ValueFile[i], "1");
//		fflush(ValueFile[i]);
//		fprintf(ValueFile[i==0?1:(i==1)?2:0],"1");
//		fflush(ValueFile[i==0?1:(i==1)?2:0]);
//        	sleep(1);
//		fprintf(ValueFile[i], "0");
//		fflush(ValueFile[i]);
//                fprintf(ValueFile[i==0?1:(i==1)?2:0],"0");
//                fflush(ValueFile[i==0?1:(i==1)?2:0]);
//		sleep(1);
//	}
	for ( i = 0; i < NUM_PINS; i++) {
    	/* Toggle each LED pair */
    		for (int j = 0; j < NUM_PINS; j++) {
        		if (i != j && i<j) { /* Avoid toggling the same LED twice */
            			fprintf(ValueFile[i], "1"); 
           			fflush(ValueFile[i]);
           			fprintf(ValueFile[j], "1");
           			fflush(ValueFile[j]);
           			sleep(1);
           			fprintf(ValueFile[i], "0"); 
           			fflush(ValueFile[i]);
           			fprintf(ValueFile[j], "0");
           			fflush(ValueFile[j]);
           			sleep(1);
        		}
    		}
	}

	/* Turn on all LEDs */
	printf("  Turn on 3leds\n");
	for(i = 0; i < NUM_PINS; i++) {
        	fprintf(ValueFile[i], "1");
        	fflush(ValueFile[i]);
    	}
    	sleep(3);
    	/* Turn off all LEDs */
	printf("  Turn off 3leds\n");
    	for(i = 0; i < NUM_PINS; i++) {
        	fprintf(ValueFile[i], "0");
        	fflush(ValueFile[i]);
    	}

	
	/* close fd */
	for(i = 0;i<NUM_PINS;i++)
    		fclose(ValueFile[i]);
	exit(EXIT_SUCCESS);
}

/*
 * Function	: TurnOffLeds
 *
 * Description	: This function turns off the LED connected to the specified GPIO pin by writing "0" to its value file.
 *
 * Arguments	: GpioPin - The GPIO pin number.
 *
 * Returns	: None.
 */
void TurnOffLeds(int GpioPin) {
    	char GpioValPath[100];
    	snprintf(GpioValPath, sizeof(GpioValPath), "/sys/class/gpio/gpio%d/value", GpioPin);
    	FILE *ValueFile = fopen(GpioValPath, "w");
    	if(ValueFile == NULL) {
        	perror("Error opening value file");
        	exit(EXIT_FAILURE);
    	}
    	fprintf(ValueFile, "0");
    	fclose(ValueFile);
}
/*
 * Function	: ToggleLed
 *
 * Description	: This function blinks the LED on the specified GPIO pin. It writes "1" to the pin's
 *                value file to turn the LED on, waits for a second, then writes "0" to turn it off,
 *                creating a blinking effect.
 *
 * Arguments	: GpioPin - The GPIO pin number.
 *
 * Returns	: None.
 */
void BlinkLed(int GpioPin) {
    	char GpioValPath[100];

    	snprintf(GpioValPath, sizeof(GpioValPath), "/sys/class/gpio/gpio%d/value", GpioPin);
	/* Open value file */
    	FILE *ValueFile = fopen(GpioValPath, "w");
    	if(ValueFile == NULL) {
        	perror("Error opening value file");
        	exit(EXIT_FAILURE);
    	}

    	for(int i=0;i<NUM_PINS;i++){
        	fprintf(ValueFile, "1");
        	fflush(ValueFile);
        	sleep(1);
        	fprintf(ValueFile, "0");
        	fflush(ValueFile);
        	sleep(1);
	}
    	fclose(ValueFile);
}
/*
 * Function	: main
 *
 * Description	: This is the main function of the program. It initializes the GPIO pins, prompts
 *                the user to select an LED to blink, and blinks the selected LED accordingly.
 *                It continues to prompt the user until the program is terminated.
 *
 * Returns	:     0 upon successful execution of the program.
 */
int main(void) {
	printf("This program demonistrates working with GPIO pins to Blink RGB Leds\n");
    	int choice;
	int8_t PIN[NUM_PINS]={17,22,27};
	/* Export and set direction for each GPIO pin */
	for(int i=0;i<NUM_PINS;i++){
		ExportGpioPin(PIN[i]);
		SetPinDirection(PIN[i], "out");
	}
    	printf("Enter LED number (1, 2, 3 or 4 to blink LEDs):");
	while(1){
    		scanf("%d", &choice);
    		switch(choice){
    	    	case 1:
    	    		printf("%d:Blink green led 3times\n",choice);
			fflush(stdout);
    	        		BlinkLed(PIN_1);
    	        		break;
    	    	case 2:
    	    		printf("%d:Blink red led 3times\n",choice);
			fflush(stdout);
    	        		BlinkLed(PIN_2);
    	        		break;
    	    	case 3:
    	    		printf("%d:Blink blue led 3times\n",choice);
			fflush(stdout);
    	        		BlinkLed(PIN_3);
    	        		break;
    	    	case 4:
				printf("%d:Blink all leds\n",choice);
				fflush(stdout);
    	        		TurnOnLeds();
    	        		break;
    	    	default:
    	        		printf("Invalid choice. Please select 1, 2,3 or 4 .\n");
    		}
	}

    	return 0;
}


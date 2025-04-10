#ifndef DATA_DISPLAY_H
#define DATA_DISPLAY_H

#pragma once
/* Include Header files */
#include "driver/gpio.h"

/* Defining GPIO pins for the LCD */
#define RS_PIN  		GPIO_NUM_26  	  /*   Register Select 	*/
#define EN_PIN  		GPIO_NUM_27  	  /*   Enable 		*/
#define D4_PIN  		GPIO_NUM_14 	  /*   Data pin 4 	*/
#define D5_PIN  		GPIO_NUM_32 	  /*   Data pin 5 	*/
#define D6_PIN  		GPIO_NUM_33 	  /*   Data pin 6 	*/
#define D7_PIN  		GPIO_NUM_25 	  /*   Data pin 7 	*/

#define LCD_RESET   		0x03  		  /*   Reset command for initialization */
#define LCD_SET_4BIT_MODE    	0x02  		  /*   Switch to 4-bit mode 		*/
#define LCD_CONFIG_4BIT_2LINE  	0x28  		  /*   4-bit, 2-line, 5x8 font 		*/
#define LCD_DISPLAY_OFF         0x08  		  /*   Turn display OFF 		*/
#define LCD_CLEAR_DISPLAY      	0x01  		  /*   Clear display 			*/
#define LCD_CURSOR_MOVE         0x06  		  /*   Cursor increment, no shift 	*/
#define LCD_DISPLAY_ON          0x0C  		  /*   Display ON, cursor OFF 		*/	
#define LCD_SECOND_LINE  	0xC0		  /*   Force cursor to beginning of 2nd row */
#define LCD_FIRST_LINE		0x80		  /*   Force cursor to beginning of 1st row */ 
#define DISPLAY_ON              0x01              /*   Display on, cursor off 	*/
#define DISPLAY_WIDTH		16		  /*   LCD Screen width   	*/
#define BUFF_SIZE		32		  /*   Buffer size 		*/

#define LCD_CMD_DELAY_300	pdMS_TO_TICKS(300)
#define LCD_CMD_DELAY_50	pdMS_TO_TICKS(50) /*   For GPIO config  */
#define LCD_CMD_DELAY_20	pdMS_TO_TICKS(20)
#define LCD_CMD_DELAY_10        pdMS_TO_TICKS(10)
#define LCD_CMD_DELAY_5   	pdMS_TO_TICKS(5)  /*   Delay for reset 	*/
#define LCD_CMD_DELAY_1  	pdMS_TO_TICKS(1)  /*   Short delay 	*/
#define LCD_CLEAR_DELAY_2      	pdMS_TO_TICKS(2)  /*   Delay after clear*/



/* Function prototypes */
void display_welcome(void);

void lcd_init(void);

void lcd_send_command(uint8_t u8Cmd);

void lcd_send_nibble(uint8_t u8Nibble);

void lcd_send_byte(uint8_t u8Byte, bool IsCmd);

void lcd_pulse_enable(void);

void lcd_print(const char *ps8Str);
void dataDisplayTask(void *pvParameters);
#endif	/* DATA_DISPLAY_H */


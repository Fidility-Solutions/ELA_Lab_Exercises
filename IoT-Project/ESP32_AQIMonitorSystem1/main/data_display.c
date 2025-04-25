/******************************************************************************
 * File        : data_display.c
 *
 * Description : This file contains a FreeRTOS task responsible for displaying 
 *               sensor data on an LCD screen. It alternates between displaying 
 *               a welcome message and continuously updating sensor readings such 
 *               as temperature, humidity, air quality, and gas resistance. 
 *               The task safely accesses shared sensor data using a semaphore.
 *
 * Author      : Fidility Solutions.
 *
 * Reference   : None.
 *******************************************************************************/

/* Include Headers */
#include "data_display.h"
#include "main.h"
#include <string.h>

static uint8_t gu8Flag = 1;


/*
 * Function     : dataDisplayTask()
 *
 * Description  : This FreeRTOS task is responsible for displaying sensor data on the LCD. 
 *                It alternates between displaying a welcome message and showing sensor 
 *                readings such as temperature, humidity, air quality, and gas resistance.
 *                The task uses a semaphore to safely access shared sensor data and 
 *                ensures proper synchronization while displaying the data on the LCD.
 *
 * Parameters   : pvParameters - A pointer to parameters passed during task creation, not used here.
 *
 * Returns      : None
 *
 */
void dataDisplayTask(void *pvParameters) 
{
	STR_SENSOR_DATA str_local_sensor_data;

	while (1)
	{
		if(gu8Flag == 1)
		{
			lcd_send_command(LCD_CLEAR_DISPLAY); /* Clear display */
			vTaskDelay(LCD_CMD_DELAY_20);

			/* Enable cursor and make it blink */
			lcd_send_command(LCD_DISPLAY_ON); /* Display ON, Cursor ON, Blink ON */

			lcd_print("Welcome to");
			vTaskDelay(LCD_CMD_DELAY_10);
			const char as8Msg[] = "Fidility Solutions  ";
			uint8_t u8MsgLen = strlen(as8Msg);

			for (int i = 0; i <= u8MsgLen; i++)
			{
				/* Move cursor to the beginning of the second line */
				lcd_send_command(LCD_SECOND_LINE);

				for (int j = 0; j < DISPLAY_WIDTH; j++)
				{
					if (i + j < u8MsgLen)
					{
						vTaskDelay(pdMS_TO_TICKS(5));
						lcd_send_byte(as8Msg[(i + j) % u8MsgLen], false);
					}
					else
					{
						lcd_send_byte(' ', false);
					}
				}
				
				vTaskDelay(LCD_CMD_DELAY_300);
			}
		}

		/* Use semaphore to access shared sensor data safely */
		if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
		{
			gu8Flag = 0;

			/* Copy shared data safely */
			memcpy(&str_local_sensor_data, &str_global_sensor_data, sizeof(STR_SENSOR_DATA));

			lcd_send_command(LCD_CLEAR_DISPLAY);
			vTaskDelay(LCD_CMD_DELAY_20);
			
			uint8_t u8CurLine1 = 0; /* Cursor for the first line */
			uint8_t u8CurLine2 = 0; /* Cursor for the second line */

			/* Print temperature & Humidity on the first line */
			char s8Buffer1[BUFF_SIZE];
			sprintf(s8Buffer1, "T:%.1f%cC,H:%.1f%%", str_local_sensor_data.bme680.f32Temperature,
					0xDF, str_local_sensor_data.bme680.f32Humidity);

			for (int i = 0; s8Buffer1[i] != '\0'; i++)
			{
				if (u8CurLine1 < DISPLAY_WIDTH)
				{
					/* Move cursor on the first line */
					lcd_send_command(LCD_FIRST_LINE + u8CurLine1);

					/* Print character */
					lcd_send_byte(s8Buffer1[i], false);
					u8CurLine1++;
				}
			}

			/* Large enough to handle a formatted string */
			char s8Buffer2[BUFF_SIZE];
			sprintf(s8Buffer2, "A:%.1f, C2:%.1fPPM",
					(double)str_local_sensor_data.air_quality_index,
					(double)str_local_sensor_data.bme680.u16GasRes);

			for (int i = 0; s8Buffer2[i] != '\0'; i++)
			{
				if (u8CurLine2 < DISPLAY_WIDTH)
				{
					/* Move cursor on the second line */
					lcd_send_command(LCD_SECOND_LINE + u8CurLine2);

					/* Print character */
					lcd_send_byte(s8Buffer2[i], false);
					u8CurLine2++;
				}
			}

			/* Semaphore Give moved to the end to ensure safe access */
			xSemaphoreGive(dataSyncSemaphore);
		}

		vTaskDelay(600 / portTICK_PERIOD_MS);
	}
}

/*
 * Function     : lcd_init()
 *
 * Description  : This function initializes the LCD in 4-bit mode using GPIOs.
 *                It sets up the control and data pins, sends the required function
 *                set commands to switch to 4-bit mode, and configures the display 
 *                settings such as number of lines, cursor behavior, and display state.
 *
 * Parameters   : None
 * 
 * Returns      : None
 *
 */
void lcd_init(void) 
{
	/* Configure GPIOs as outputs */
	gpio_config_t io_conf = 
	{
		.pin_bit_mask = (1ULL << RS_PIN) | (1ULL << EN_PIN) |
			(1ULL << D4_PIN) | (1ULL << D5_PIN) |
			(1ULL << D6_PIN) | (1ULL << D7_PIN),
		.mode = GPIO_MODE_OUTPUT,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf);

	vTaskDelay(LCD_CMD_DELAY_50); /* Wait for LCD to power up */

	/* Function set commands */
	lcd_send_nibble(LCD_RESET);
	vTaskDelay(LCD_CMD_DELAY_5);
	lcd_send_nibble(LCD_RESET);
	
	vTaskDelay(LCD_CMD_DELAY_1);
	
	lcd_send_nibble(LCD_RESET);
	vTaskDelay(LCD_CMD_DELAY_1);

	/* Switch to 4-bit mode */
	lcd_send_nibble(LCD_SET_4BIT_MODE);

	/* Configure LCD for 4-bit mode */
	lcd_send_command(LCD_CONFIG_4BIT_2LINE); 	/* Function set: 4-bit, 2-line, 5x8 dots */
	lcd_send_command(LCD_DISPLAY_OFF); 		/* Display OFF */
	lcd_send_command(LCD_CLEAR_DISPLAY); 		/* Clear display*/
	
	vTaskDelay(LCD_CLEAR_DELAY_2); 			/* Wait for clearing */
	
	lcd_send_command(LCD_CURSOR_MOVE); 		/* Entry mode set: Increment cursor */
	lcd_send_command(LCD_DISPLAY_ON); 		/* Display ON, Cursor OFF */
}


/*
 * Function     : lcd_send_command()
 *
 * Description  : This function sends a command byte to the LCD.
 *                It wraps the lcd_send_byte() function with the RS (Register Select)
 *                flag set to true, indicating that the byte is a command.
 *
 * Parameters   : cmd - The command byte to be sent to the LCD.
 *
 * Returns      : None
 *
 */
void lcd_send_command(uint8_t u8Cmd) 
{
	lcd_send_byte(u8Cmd, true);
}

/*
 * Function     : lcd_send_byte()
 *
 * Description  : Sends a byte to the LCD in 4-bit mode by splitting it into two nibbles.
 *                It sets the RS (Register Select) pin based on whether the byte is a 
 *                command or data, then sends the high and low nibbles sequentially.
 *
 * Parameters   : byte       - The 8-bit value to be sent to the LCD.
 *                is_command - Boolean flag indicating command (true) or data (false).
 * 
 * Returns      : None
 *
 */
void lcd_send_byte(uint8_t u8Byte, bool IsCmd) 
{
	gpio_set_level(RS_PIN, IsCmd ? 0 : 1); 	/* Command or data mode */
	lcd_send_nibble(u8Byte >> 4);                	/* Send higher nibble */
	lcd_send_nibble(u8Byte & 0x0F);              	/* Send lower nibble */
}

/*
 * Function     : lcd_send_nibble()
 *
 * Description  : Sends a 4-bit nibble to the LCD by setting the corresponding GPIO pins.
 *                After setting the data lines (D4 to D7), it pulses the enable pin to
 *                latch the data into the LCD.
 *
 * Parameters   : u8Nibble - The lower 4 bits of the byte to be sent to the LCD.
 *
 * Returns      : None
 *
 */
void lcd_send_nibble(uint8_t u8Nibble) 
{
	gpio_set_level(D4_PIN, (u8Nibble >> 0) & 0x01);
	gpio_set_level(D5_PIN, (u8Nibble >> 1) & 0x01);
	gpio_set_level(D6_PIN, (u8Nibble >> 2) & 0x01);
	gpio_set_level(D7_PIN, (u8Nibble >> 3) & 0x01);
	lcd_pulse_enable();
}

/*
 * Function     : lcd_pulse_enable()
 *
 * Description  : Generates an enable pulse to latch data or command into the LCD.
 *                It sets the EN pin high briefly and then sets it low, ensuring
 *                the required delays are met as per LCD timing specifications.
 *
 * Parameters   : None
 * 
 * Returns      : None
 *
 */
void lcd_pulse_enable() 
{
	gpio_set_level(EN_PIN, 1); /* Enable high */
	esp_rom_delay_us(1);       /* Wait > 450ns */
	
	gpio_set_level(EN_PIN, 0); /* Enable low */
	esp_rom_delay_us(50);      /* Wait > 37µs for execution */
}

/*
 * Function     : lcd_print()
 *
 * Description  : Prints a null-terminated string on the LCD by sending each character 
 *                as data using the lcd_send_byte() function.
 *
 * Parameters   : ps8Str - Pointer to the null-terminated string to be displayed on the LCD.
 * 
 * Returns      : None
 *
 */
void lcd_print(const char *ps8Str) 
{
	while (*ps8Str) 
	{
		lcd_send_byte(*ps8Str, false); 	/* Send each character as data */
		ps8Str++;
	}
}

/*
 * Function     : lcd_putch()
 *
 * Description  : Sends a single character to the LCD as data using the lcd_send_byte() function.
 *
 * Parameters   : s8Char - The character to be displayed on the LCD.
 *
 * Returns      : None
 *
 */
void lcd_putch(char s8Char) 
{
	lcd_send_byte(s8Char, false); 		/* Send single character as data */
}

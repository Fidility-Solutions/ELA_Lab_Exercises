#include "data_display.h"

static uint8_t flag = 1;

/*
 * This task handles displaying sensor data on an LCD.
 *
 * 1. Display Welcome Message:
 *    - The task first shows a welcome message on the LCD screen (e.g., "Welcome").
 *
 * 2. Display Sensor Data:
 *    - After displaying the welcome message, the task updates the LCD with sensor data.
 *    - The first line of the LCD displays the temperature and humidity values.
 *    - The second line of the LCD displays the AQI (Air Quality Index) and CO2 levels.
 *    - Data is updated periodically to reflect the most recent sensor readings.
 *
 * 3. Task Delay:
 *    - Wait for a short period (e.g., 500ms or based on a defined update rate) before refreshing the display.
 */


void dataDisplayTask(void *pvParameters) 
{
	STR_SENSOR_DATA local_copy;  /* Local copy for safe access */

	while (1)
	{
		if(flag == 1)
		{
			lcd_send_command(LCD_CLEAR_DISPLAY); /* Clear display */
			vTaskDelay(LCD_CMD_DELAY_20);

			/* Enable cursor and make it blink */
			lcd_send_command(LCD_DISPLAY_ON); /* Display ON, Cursor ON, Blink ON */

			lcd_print("Welcome to");
			vTaskDelay(LCD_CMD_DELAY_10);
			const char message[] = "Fidility Solutions  ";
			int message_length = strlen(message);

			for (int i = 0; i <= message_length; i++)
			{
				/* Move cursor to the beginning of the second line */
				lcd_send_command(LCD_SECOND_LINE);

				for (int j = 0; j < DISPLAY_WIDTH; j++)
				{
					if (i + j < message_length)
					{
						vTaskDelay(pdMS_TO_TICKS(5));
						lcd_send_byte(message[(i + j) % message_length], false);
					}
					else
					{
						lcd_send_byte(' ', false);
					}
				}
				vTaskDelay(LCD_CMD_DELAY_300);
			}
		}

		// Use semaphore to access shared sensor data safely
		if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
		{
			flag = 0;

			/*ESP_LOGI("DISPLAYING", "GOING TO STORE: Temp = %.2f°C, Hum = %.2f%%, Gas Resistance = %.2f ohms, "
					"AQI = %d, Time = %s, PM2.5 = %.1f, PM10 = %.1f, Location = %s\033[0m",
						(double)global_sensor_data.bme680.temperature,
						(double)global_sensor_data.bme680.humidity,
						(double)global_sensor_data.bme680.gas_resistance / 100.0,
						global_sensor_data.air_quality_index,
						global_sensor_data.time_str,
						(double)global_sensor_data.sds011.pm25 / 10.0,
						(double)global_sensor_data.sds011.pm10 / 10.0,
						global_sensor_data.location);*/


			// Copy shared data safely
			memcpy(&local_copy, &global_sensor_data, sizeof(STR_SENSOR_DATA));

			lcd_send_command(LCD_CLEAR_DISPLAY);
			vTaskDelay(LCD_CMD_DELAY_20);
			int cursor_line1 = 0; /* Cursor for the first line */
			int cursor_line2 = 0; /* Cursor for the second line */

			/* Print temperature & Humidity on the first line */
			char buffer1[32];
			sprintf(buffer1, "T:%.1f%cC,H:%.1f%%", local_copy.bme680.f32Temperature,
					0xDF, local_copy.bme680.f32Humidity);

			for (int i = 0; buffer1[i] != '\0'; i++)
			{
				if (cursor_line1 < DISPLAY_WIDTH)
				{
					/* Move cursor on the first line */
					lcd_send_command(LCD_FIRST_LINE + cursor_line1);

					/* Print character */
					lcd_send_byte(buffer1[i], false);
					cursor_line1++;
				}
			}

			/* Large enough to handle a formatted string */
			char buffer2[32];
			sprintf(buffer2, "A:%.1f, C2:%.1fPPM",
					(double)local_copy.sds011.u16PM25Val,
					(double)local_copy.bme680.u16GasRes);

			for (int i = 0; buffer2[i] != '\0'; i++)
			{
				if (cursor_line2 < DISPLAY_WIDTH)
				{
					/* Move cursor on the second line */
					lcd_send_command(LCD_SECOND_LINE + cursor_line2);

					/* Print character */
					lcd_send_byte(buffer2[i], false);
					cursor_line2++;
				}
			}

			// **Semaphore Give moved to the end to ensure safe access**
			xSemaphoreGive(dataSyncSemaphore);
		}

		vTaskDelay(600 / portTICK_PERIOD_MS);
	}
}


void lcd_init() 
{
	/* Configure GPIOs as outputs */
	gpio_config_t io_conf = {
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



void lcd_send_command(uint8_t cmd) 
{
	lcd_send_byte(cmd, true);
}

void lcd_send_byte(uint8_t byte, bool is_command) 
{
	gpio_set_level(RS_PIN, is_command ? 0 : 1); 	/* Command or data mode */
	lcd_send_nibble(byte >> 4);                	/* Send higher nibble */
	lcd_send_nibble(byte & 0x0F);              	/* Send lower nibble */
}


void lcd_send_nibble(uint8_t nibble) 
{
	gpio_set_level(D4_PIN, (nibble >> 0) & 0x01);
	gpio_set_level(D5_PIN, (nibble >> 1) & 0x01);
	gpio_set_level(D6_PIN, (nibble >> 2) & 0x01);
	gpio_set_level(D7_PIN, (nibble >> 3) & 0x01);
	lcd_pulse_enable();
}


void lcd_pulse_enable() 
{
	gpio_set_level(EN_PIN, 1); /* Enable high */
	esp_rom_delay_us(1);       /* Wait >450ns */
	gpio_set_level(EN_PIN, 0); /* Enable low */
	esp_rom_delay_us(50);      /* Wait >37µs for execution */
}


void lcd_print(const char *str) 
{
	while (*str) 
	{
		lcd_send_byte(*str, false); 	/* Send each character as data */
		str++;
	}
}

void lcd_putch(char ch) 
{
	lcd_send_byte(ch, false); 		/* Send single character as data */
}

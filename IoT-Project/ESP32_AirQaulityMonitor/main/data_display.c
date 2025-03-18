#include "main.h"
#include "data_display.h"
#include "driver/gpio.h"
#include "rtc.h"
#include "time.h"
#include <sys/time.h>


/* Defining GPIO pins for the LCD */
#define RS_PIN  GPIO_NUM_26  /* Register Select */
#define EN_PIN  GPIO_NUM_27  /* Enable */
#define D4_PIN  GPIO_NUM_14 /* Data pin 4 */
#define D5_PIN  GPIO_NUM_32 /* Data pin 5 */
#define D6_PIN  GPIO_NUM_33 /* Data pin 6 */
#define D7_PIN  GPIO_NUM_25 /* Data pin 7 */


static int flag = 1;


void lcd_init();
void lcd_send_command(uint8_t cmd);
void lcd_send_nibble(uint8_t nibble);
void lcd_send_byte(uint8_t byte, bool is_command);
void lcd_pulse_enable();
void lcd_print(const char *str);


void dataDisplayTask(void *pvParameters) 
{

	while (1)
	{
		if(flag == 1)
		{
			lcd_send_command(0x01); /* Clear display */
			vTaskDelay(pdMS_TO_TICKS(20));

			/* Enable cursor and make it blink */
			lcd_send_command(0x0F); /* Display ON, Cursor ON, Blink ON */

			lcd_print("Welcome to");
			vTaskDelay(pdMS_TO_TICKS(10));
			const char message[] = "Fidility Solutions  "; 
			int message_length = strlen(message);         
			int display_width = 16;                       

			for (int i = 0; i <= message_length; i++) 
			{
				lcd_send_command(0xC0); /* Move cursor to the beginning of the second line */

				for (int j = 0; j < display_width; j++)
				{
					if (i + j < message_length) {
						vTaskDelay(pdMS_TO_TICKS(5));
						lcd_send_byte(message[(i + j) % message_length], false);
					} else {
						lcd_send_byte(' ', false);
					}
				}
				vTaskDelay(pdMS_TO_TICKS(300));
			}
		}
	        if (xQueuePeek(sensorDataQueue, &global_sensor_data, portMAX_DELAY) == pdTRUE) 
		{
			flag = 0;
			if (xSemaphoreTake(dataSyncSemaphore, portMAX_DELAY))
			{
				lcd_send_command(0x01);
				vTaskDelay(pdMS_TO_TICKS(20));
				int cursor_line1 = 0; /* Cursor for the first line */
				int cursor_line2 = 0; /* Cursor for the second line */

				// Print temperature on the first line
				char buffer1[32];
				sprintf(buffer1, "T:%.1f%cC,H:%.1f%%", global_sensor_data.bme680.temperature,
						0xDF, global_sensor_data.bme680.humidity);
				for (int i = 0; buffer1[i] != '\0'; i++)
				{
					if (cursor_line1 < 16)
					{
						lcd_send_command(0x80 + cursor_line1); /* Move cursor on the first line */
						lcd_send_byte(buffer1[i], false); /* Print character */
						cursor_line1++;
					}
				}
				char buffer2[32]; /* Large enough to handle a formatted string */
				sprintf(buffer2, "A:%f,C2:%.1fPPM", global_sensor_data.sds011.pm25, 
						global_sensor_data.bme680.gas_resistance);
				for (int i = 0; buffer2[i] != '\0'; i++)
				{
					if (cursor_line2 < 16)
					{
						lcd_send_command(0xC0 + cursor_line2); /* Move cursor on the second line */
						lcd_send_byte(buffer2[i], false);     /* Print character */
						cursor_line2++;
					}
				}

 			 xSemaphoreGive(dataSyncSemaphore);

			}
		}
		vTaskDelay(500 / portTICK_PERIOD_MS);
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

	vTaskDelay(pdMS_TO_TICKS(50)); /* Wait for LCD to power up */

	/* Function set commands */
	lcd_send_nibble(0x03);
	vTaskDelay(pdMS_TO_TICKS(5));
	lcd_send_nibble(0x03);
	vTaskDelay(pdMS_TO_TICKS(1));
	lcd_send_nibble(0x03);
	vTaskDelay(pdMS_TO_TICKS(1));

	/* Switch to 4-bit mode */
	lcd_send_nibble(0x02);

	// Configure LCD for 4-bit mode
	lcd_send_command(0x28); /* Function set: 4-bit, 2-line, 5x8 dots */
	lcd_send_command(0x08); /* Display OFF */
	lcd_send_command(0x01); /* Clear display*/
	vTaskDelay(pdMS_TO_TICKS(2)); /* Wait for clearing */
	lcd_send_command(0x06); /* Entry mode set: Increment cursor */
	lcd_send_command(0x0C); /* Display ON, Cursor OFF */
}



void lcd_send_command(uint8_t cmd) {
	lcd_send_byte(cmd, true);
}

void lcd_send_byte(uint8_t byte, bool is_command) {
	gpio_set_level(RS_PIN, is_command ? 0 : 1); /* Command or data mode */
	lcd_send_nibble(byte >> 4);                /* Send higher nibble */
	lcd_send_nibble(byte & 0x0F);              /* Send lower nibble */
}


void lcd_send_nibble(uint8_t nibble) {
	gpio_set_level(D4_PIN, (nibble >> 0) & 0x01);
	gpio_set_level(D5_PIN, (nibble >> 1) & 0x01);
	gpio_set_level(D6_PIN, (nibble >> 2) & 0x01);
	gpio_set_level(D7_PIN, (nibble >> 3) & 0x01);
	lcd_pulse_enable();
}


void lcd_pulse_enable() {
	gpio_set_level(EN_PIN, 1); /* Enable high */
	esp_rom_delay_us(1);       /* Wait >450ns */
	gpio_set_level(EN_PIN, 0); /* Enable low */
	esp_rom_delay_us(50);      /* Wait >37µs for execution */
}


void lcd_print(const char *str) {
	while (*str) {
		lcd_send_byte(*str, false); /* Send each character as data */
		str++;
	}
}

void lcd_putch(char ch) {
	lcd_send_byte(ch, false); /* Send single character as data */
}

#include "main.h"
#include "esp_log.h"
#include "time.h"
#include "sys/time.h"
#include "rtc.h"

#define MAX_TIME_STR_LENGTH 20


void init_RTC()
{
	struct tm timeinfo;
	timeinfo.tm_year = 2025 - 1900; // Year since 1900
	timeinfo.tm_mon = 3 - 1;        // Month (1-12, 0-based)
	timeinfo.tm_mday = 18;           // Day of the month
	timeinfo.tm_hour = 11;          // Hour (24-hour format)
	timeinfo.tm_min = 0;            // Minute
	timeinfo.tm_sec = 0;            // Second

	struct timeval tv;
	tv.tv_sec = mktime(&timeinfo);  // Convert struct tm to seconds since Unix epoch
	tv.tv_usec = 0;                 // Set microseconds to 0
	settimeofday(&tv, NULL);        // Set the system time
	ESP_LOGI("Time", "Time set to: %04d-%02d-%02d %02d:%02d:%02d",
			timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
			timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}


void get_time() {
	struct tm timeinfo;
	struct timeval tv;


	if (gettimeofday(&tv, NULL) == 0) 
	{
		localtime_r(&tv.tv_sec, &timeinfo); // Convert seconds since the epoch to local time
		int len = snprintf(global_sensor_data.time_str, MAX_TIME_STR_LENGTH,
				"%04d-%02d-%02d %02d:%02d:%02d",
				timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
				timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

		// Ensure that the string is null-terminated and doesn't exceed buffer size
		if (len >= MAX_TIME_STR_LENGTH) {
			ESP_LOGE("Time", "Formatted time string is too long, truncating.");
			global_sensor_data.time_str[MAX_TIME_STR_LENGTH - 1] = '\0';  // Truncate to fit
		}
	}

	else {
		ESP_LOGE("Time", "Failed to get current time");
	}
}

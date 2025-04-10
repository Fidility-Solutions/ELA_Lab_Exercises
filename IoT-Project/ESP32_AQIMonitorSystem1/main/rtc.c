/******************************************************************************
 * File         : rtc.c
 *
 * Description  : This file contains the logic for handling the Real-Time 
 *                Clock (RTC) and its synchronization with NTP (Network Time 
 *                Protocol). It ensures that the system time is correctly 
 *                updated either via NTP or fallback to the RTC in case NTP 
 *                synchronization is unavailable. The file includes functions 
 *                for initializing RTC, updating the system time, and ensuring 
 *                that the time remains accurate over time.
 *
 * Author       : Fidility Solutions.
 *
 * References   : ESP-IDF RTC API, SNTP (Simple Network Time Protocol), 
 *                AWS IoT MQTT Documentation.
 *
 * Key Functions:
 *  - init_RTC: Initializes the RTC with a predefined time (e.g., 2025-03-18 11:00:00). 
 *              This sets up the system time if NTP is not available.
 *  - update_current_time: Updates the current time using NTP or RTC, depending 
 *                          on whether NTP is synchronized.
 *  - update_time_with_fallback: Checks for NTP synchronization and updates 
 *                                the time accordingly. If NTP is unavailable, 
 *                                the RTC time is used as a fallback.
 *  - get_time: Retrieves the current time from the RTC and formats it as a 
 *              string (YYYY-MM-DD HH:MM:SS), storing the result in the global 
 *              `str_global_sensor_data.atime_str` variable.
 *
 * Purpose      : The purpose of this module is to ensure that the system time 
 *                is always accurate by synchronizing it with a remote NTP 
 *                server. If NTP synchronization fails, it uses the RTC to 
 *                maintain system time until NTP becomes available again.
 ******************************************************************************/
/* Include Header files */
#include "rtc.h"
#include "main.h"
#include "esp_log.h"
#include <time.h>
#include "esp_sntp.h"

#include "esp_netif.h"

volatile bool isNTPSynched = false;


/******************************************************************************
 * Function     : initialize_sntp
 *
 * Description  : Initializes the Simple Network Time Protocol (SNTP) client
 *                for time synchronization. It configures the NTP operating
 *                mode and sets the NTP server to "time.google.com". The NTP
 *                client is then initialized to begin time synchronization.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Note         : The function uses Google's public NTP server ("time.google.com")
 *                for time synchronization. The SNTP client will poll the server
 *                to obtain the current time.
 ******************************************************************************/
void initialize_sntp(void)
{
        ESP_LOGI(NTP_TAG, "Initializing SNTP...");
	esp_netif_init();
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "time.google.com");

        /* Initialize the NTP protocol */
        esp_sntp_init();
}

/******************************************************************************
 * Function     : init_ntp
 *
 * Description  : Initializes the NTP client and synchronizes the system time 
 *                with an NTP server. The function waits for the time to be 
 *                successfully synchronized, retries up to 10 times if needed, 
 *                and then sets the system timezone to IST (Indian Standard Time, UTC +5:30).
 *                Upon successful synchronization, the function also updates the
 *                system's Real-Time Clock (RTC) with the NTP time.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Note         : The function checks if the year obtained from the NTP server 
 *                is valid (greater than 2016). If not, the function retries a 
 *                maximum of 10 times. Upon success, it updates the global sensor 
 *                data with the synchronized time and prints the time in IST.
 ******************************************************************************/
void init_ntp(void) 
{
	/* Initialize SNTP */
	ESP_LOGI(NTP_TAG, "Starting NTP initialization...");

	initialize_sntp();

	/* Wait for time to be set */
	time_t now = 0;
	struct tm timeinfo = {0};
	int retry = 0;
	const int max_retries = 10;

	while (timeinfo.tm_year < (2016 - 1900) && ++retry <= max_retries) 
	{
		ESP_LOGI(NTP_TAG, "Waiting for system time to be set... (%d/%d)", retry, max_retries);
		vTaskDelay(pdMS_TO_TICKS(2000)); /* Wait 2 seconds */

		time(&now);
		localtime_r(&now, &timeinfo);
	}

	if (timeinfo.tm_year < (2016 - 1900)) 
	{
		ESP_LOGW(NTP_TAG, "Failed to synchronize time with NTP server.");
		isNTPSynched = false;
		return;
	}

	/* Set timezone to IST (UTC +5:30) */
	setenv("TZ", "IST-5:30", 1);
	tzset();

	isNTPSynched = true;  /* Mark NTP as synchronized */

	/* Print the synchronized time in IST */
	char strftime_buf[64];
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	strncpy(str_global_sensor_data.atime_str, strftime_buf, MAX_TIME_STR_LENGTH - 1);
	str_global_sensor_data.atime_str[MAX_TIME_STR_LENGTH - 1] = '\0';  // Ensure null-termination

	/* Synchronize RTC with NTP time */
	struct timeval tv;
	tv.tv_sec = now;
	tv.tv_usec = 0;
	settimeofday(&tv, NULL);

	ESP_LOGI(NTP_TAG, "RTC synchronized with NTP time.");
}

/******************************************************************************
 * Function     : update_current_time
 *
 * Description  : Retrieves the current system time, converts it to local time 
 *                (IST, as set in `init_ntp`), formats it as "YYYY-MM-DD HH:MM:SS",
 *                and updates the global sensor data structure (`str_global_sensor_data`) 
 *                with the formatted time string. The function also logs the raw UTC 
 *                time and the updated local time for debugging purposes.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Note         : This function assumes that the NTP synchronization has already 
 *                been performed (via `init_ntp`). It retrieves and formats the 
 *                current time into a string, which is stored in `str_global_sensor_data.atime_str`.
 *                It also logs both raw UTC time and the formatted local time for debugging purposes.
 ******************************************************************************/
void update_current_time(void) 
{
	time_t now;
	struct tm timeinfo;

	/* Get the current system time */
	time(&now);

	/* Log the raw UTC time for debugging */
	ESP_LOGI(NTP_TAG, "Raw UTC time: %s", ctime(&now));

	/* Convert to local time (IST, as set in init_ntp()) */
	localtime_r(&now, &timeinfo);

	/* Format the time as "YYYY-MM-DD HH:MM:SS" and store it in str_global_sensor_data */
	char strftime_buf[64];
	strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
	strncpy(str_global_sensor_data.atime_str, strftime_buf, MAX_TIME_STR_LENGTH - 1);
	str_global_sensor_data.atime_str[MAX_TIME_STR_LENGTH - 1] = '\0';  // Ensure null-termination

	ESP_LOGI(NTP_TAG, "Updated date/time in IST is: %s", str_global_sensor_data.atime_str);
}


/******************************************************************************
 * Function     : init_RTC
 *
 * Description  : Initializes the Real-Time Clock (RTC) with a specific date and 
 *                time (March 18, 2025, 11:00:00). The function sets the system 
 *                time using the `settimeofday` function and logs the initialized 
 *                time in the format "YYYY-MM-DD HH:MM:SS".
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Note         : The time is set using a `struct tm` object that represents 
 *                March 18, 2025, at 11:00:00. The function assumes that the 
 *                system is running with a Unix-based time representation.
 *                The time is initialized in UTC.
 ******************************************************************************/
void init_RTC() 
{
	struct tm timeinfo;
	timeinfo.tm_year = 2025 - 1900;     /* Year since 2025 */
	timeinfo.tm_mon = 3 - 1;            /* Month (1-12, 0-based) */
	timeinfo.tm_mday = 18;              /* Day of the month */
	timeinfo.tm_hour = 11;              /* Hour (24-hour format) */
	timeinfo.tm_min = 0;                /* Minute */
	timeinfo.tm_sec = 0;                /* Second */

	struct timeval tv;
	tv.tv_sec = mktime(&timeinfo);  /* Convert struct tm to seconds since Unix epoch */
	tv.tv_usec = 0;                 /* Set microseconds to 0 */
	settimeofday(&tv, NULL);        /* Set the system time */

	ESP_LOGI(RTC_TAG, "RTC initialized with time: %04d-%02d-%02d %02d:%02d:%02d",
			timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
			timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}


/******************************************************************************
 * Function     : update_time_with_fallback
 *
 * Description  : Updates the system time either by using the synchronized NTP time
 *                or by falling back to the RTC if NTP is unavailable. If NTP is
 *                synchronized, it updates the time and synchronizes the RTC with
 *                the NTP time. If NTP synchronization is unavailable, the function
 *                logs a warning and uses the RTC to get the time.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Note         : The function first checks if NTP synchronization is available
 *                (`isNTPSynched`). If true, it updates the system time using
 *                NTP. If false, it falls back to using the RTC for time retrieval.
 *                This ensures that the system time is always updated, either
 *                through NTP or RTC.
 ******************************************************************************/
void update_time_with_fallback(void) 
{
	if (isNTPSynched) 
	{
		update_current_time();

		/* Synchronize RTC with NTP time */
		struct timeval tv;
		time_t now;

		time(&now);
		tv.tv_sec = now;
		tv.tv_usec = 0;
		settimeofday(&tv, NULL);

		ESP_LOGI(RTC_TAG, "RTC synchronized with NTP time.");
	} 
	else 
	{
		ESP_LOGW(RTC_TAG, "NTP unavailable. Falling back to RTC.");
		get_time();
	}
}

/******************************************************************************
 * Function     : get_time
 *
 * Description  : Retrieves the current time from the system's RTC (Real-Time Clock).
 *                The function uses `gettimeofday` to fetch the current time in
 *                seconds since the epoch and converts it to local time. It then
 *                formats the time as a string in the format "YYYY-MM-DD HH:MM:SS"
 *                and stores it in the global sensor data structure (`str_global_sensor_data.atime_str`).
 *                If the time string exceeds the buffer size, it is truncated. The function logs
 *                the formatted time string for debugging.
 *
 * Parameters   : None
 *
 * Returns      : None
 *
 * Note         : This function is typically used to retrieve the time when NTP synchronization
 *                is not available. It relies on the RTC for the current date/time.
 *                The formatted time string is stored in `str_global_sensor_data.atime_str`.
 ******************************************************************************/
void get_time() 
{
	struct tm timeinfo;
	struct timeval tv;

	if (gettimeofday(&tv, NULL) == 0) 
	{
		localtime_r(&tv.tv_sec, &timeinfo); /* Convert seconds since the epoch to local time */
		uint8_t u8Len = snprintf(str_global_sensor_data.atime_str, MAX_TIME_STR_LENGTH,
				"%04d-%02d-%02d %02d:%02d:%02d",
				timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
				timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

		/* Ensure that the string is null-terminated and doesn't exceed buffer size */
		if (u8Len >= MAX_TIME_STR_LENGTH) 
		{
			ESP_LOGE(RTC_TAG, "Formatted time string is too long, truncating.");
			str_global_sensor_data.atime_str[MAX_TIME_STR_LENGTH - 1] = '\0';  /* Truncate to fit */
		}

		ESP_LOGI(RTC_TAG, "RTC-provided date/time: %s", str_global_sensor_data.atime_str);
	} 
	else 
	{
		ESP_LOGE(RTC_TAG, "Failed to get current time");
	}
}


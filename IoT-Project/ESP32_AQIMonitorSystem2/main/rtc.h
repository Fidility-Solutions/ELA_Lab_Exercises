#ifndef RTC_H
#define RTC_H

/* Header files */
#include <stdbool.h>

/* Macro Defininitions */
#define NTP_TAG		"NTP"
#define RTC_TAG		"TIME"
#define MAX_TIME_STR_LENGTH 20


/* Function prototypes */
void init_ntp(void);

void init_RTC(void);

void get_time(void);

void update_current_time(void);

extern volatile bool isNTPSynched;

#endif

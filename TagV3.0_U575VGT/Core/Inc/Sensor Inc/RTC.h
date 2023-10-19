/*
 * RTC.h
 *
 *  Created on: Sep 18, 2023
 *      Author: kevin ma
 */

#ifndef INC_SENSOR_INC_RTC_H_
#define INC_SENSOR_INC_RTC_H_

#include "Lib Inc/timing.h"
#include "tx_api.h"
#include "main.h"
#include <stdint.h>
#include <stdbool.h>

// rtc timeout settings
#define RTC_SHUTDOWN_LIMIT_MINS 20
#define RTC_SHUTDOWN_LIMIT_HOUR 1

// rtc initialization timeout
#define RTC_GPS_TIMEOUT_MINS 1 // timeout in minutes before initializing rtc without gps timestamp

// rtc new audio file interval
#define RTC_AUDIO_REFRESH_MINS 5
#define RTC_AUDIO_REFRESH_TOL 1

// rtc event flags
#define RTC_INIT_FLAG 0x1

void rtc_thread_entry(ULONG thread_input);
void RTC_Init(RTC_TimeTypeDef *eTime, RTC_DateTypeDef *eDate);

#endif /* INC_SENSOR_INC_RTC_H_ */

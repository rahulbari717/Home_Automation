/*
 * stm32f446xx_rtc_driver.h
 *
 *  Created on: Jan 14, 2026
 *      Author: Rahul Bari.
 */

#ifndef INC_STM32F446XX_RTC_DRIVER_H_
#define INC_STM32F446XX_RTC_DRIVER_H_

#include "stm32f446xx.h"

/* Config Structure */
typedef struct {
    uint8_t RTC_HourFormat; // 12H or 24H
    // Add other configs if needed
} RTC_Config_t;

typedef struct {
    RTC_RegDef_t *pRTC;
    RTC_Config_t RTC_Config;
} RTC_Handle_t;

/* APIs supported by this driver */
void RTC_Init(RTC_Handle_t *pRTCHandle);
void RTC_SetTime(RTC_Handle_t *pRTCHandle, uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_SetDate(RTC_Handle_t *pRTCHandle, uint8_t year, uint8_t month, uint8_t date, uint8_t weekday);
void RTC_GetTime(RTC_Handle_t *pRTCHandle, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void RTC_GetDate(RTC_Handle_t *pRTCHandle, uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *weekday);

#endif /* INC_STM32F446XX_RTC_DRIVER_H_ */

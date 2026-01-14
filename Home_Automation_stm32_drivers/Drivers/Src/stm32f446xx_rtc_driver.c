/*
 * stm32f446xx_rtc_driver.c
 *
 *  Created on: Jan 14, 2026
 *      Author: Rahul Bari.
 */

/*
 * Drivers/Src/stm32f446xx_rtc_driver.c
 */
#include "stm32f446xx_rtc_driver.h"

// --- Private Helper Functions ---
static uint8_t Dec_to_BCD(uint8_t dec)
{
    return ((dec / 10) << 4) | (dec % 10);
}

static uint8_t BCD_to_Dec(uint8_t bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// --- API Functions ---

void RTC_Init(RTC_Handle_t *pRTCHandle)
{
    RCC_RegDef_t *pRCC = RCC;
    RTC_RegDef_t *pRTC = pRTCHandle->pRTC;

    // 1. Enable PWR Peripheral Clock
    pRCC->APB1ENR |= (1 << 28);

    // 2. Allow Access to Backup Domain (DBP Bit)
    PWR->CR |= (1 << 8);

    // ====================================================
    // CRITICAL FIX: Force Backup Domain Reset
    // This clears the "stuck" state from previous runs
    // ====================================================
    pRCC->BDCR |= (1 << 16); // Set BDRST (Reset)
    pRCC->BDCR &= ~(1 << 16); // Clear BDRST (Release Reset)

    // 3. Enable LSI (Internal Low Speed Oscillator)
    // We use LSI because Nucleo boards DO NOT have the LSE Crystal soldered.
    pRCC->CSR |= (1 << 0); // LSION
    while(!(pRCC->CSR & (1 << 1))); // Wait for LSIRDY (LSI Ready)

    // 4. Select LSI as RTC Clock Source
    // RTCSEL[1:0] -> 10 for LSI
    pRCC->BDCR &= ~(0x3 << 8); // Clear bits
    pRCC->BDCR |= (1 << 9);    // Set bit 9

    // 5. Enable RTC Clock
    pRCC->BDCR |= (1 << 15); // RTCEN

    // 6. Unlock Write Protection
    pRTC->WPR = 0xCA;
    pRTC->WPR = 0x53;

    // 7. Enter Init Mode (to program prescalers)
    pRTC->ISR |= (1 << 7); // INIT bit
    while(!(pRTC->ISR & (1 << 6))); // Wait for INITF (Init Flag)

    // 8. Configure Prescalers for 32kHz LSI
    // Formula: f_rtc = 32000 / ((Async+1) * (Sync+1))
    // We want 1Hz.
    // Async = 127
    // Sync = 249
    // 32000 / (128 * 250) = 1Hz
    pRTC->PRER = (127 << 16) | 249;

    // 9. Exit Init Mode (Start the Clock)
    pRTC->ISR &= ~(1 << 7);

    // 10. Enable Bypass Shadow Registers
    // This ensures we read the registers directly immediately,
    // fixing the "garbage value" issue if shadow sync fails.
    pRTC->CR |= (1 << 5);

    // 11. Enable Write Protection
    pRTC->WPR = 0xFF;
}

void RTC_SetTime(RTC_Handle_t *pRTCHandle, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    RTC_RegDef_t *pRTC = pRTCHandle->pRTC;

    pRTC->WPR = 0xCA;
    pRTC->WPR = 0x53;

    pRTC->ISR |= (1 << 7); // Enter Init
    while(!(pRTC->ISR & (1 << 6)));

    uint32_t tr = 0;
    tr |= (Dec_to_BCD(hours) << 16);
    tr |= (Dec_to_BCD(minutes) << 8);
    tr |= Dec_to_BCD(seconds);
    pRTC->TR = tr; // Write Time

    pRTC->ISR &= ~(1 << 7); // Exit Init
    pRTC->WPR = 0xFF;
}

void RTC_SetDate(RTC_Handle_t *pRTCHandle, uint8_t year, uint8_t month, uint8_t date, uint8_t weekday)
{
    RTC_RegDef_t *pRTC = pRTCHandle->pRTC;

    pRTC->WPR = 0xCA;
    pRTC->WPR = 0x53;

    pRTC->ISR |= (1 << 7);
    while(!(pRTC->ISR & (1 << 6)));

    uint32_t dr = 0;
    dr |= (Dec_to_BCD(year) << 16);
    dr |= (weekday << 13);
    dr |= (Dec_to_BCD(month) << 8);
    dr |= Dec_to_BCD(date);
    pRTC->DR = dr; // Write Date

    pRTC->ISR &= ~(1 << 7);
    pRTC->WPR = 0xFF;
}

void RTC_GetTime(RTC_Handle_t *pRTCHandle, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    RTC_RegDef_t *pRTC = pRTCHandle->pRTC;
    uint32_t tr = pRTC->TR;

    *hours = BCD_to_Dec((tr >> 16) & 0x3F);
    *minutes = BCD_to_Dec((tr >> 8) & 0x7F);
    *seconds = BCD_to_Dec(tr & 0x7F);
}

void RTC_GetDate(RTC_Handle_t *pRTCHandle, uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *weekday)
{
    RTC_RegDef_t *pRTC = pRTCHandle->pRTC;
    uint32_t dr = pRTC->DR;

    *year = BCD_to_Dec((dr >> 16) & 0xFF);
    *month = BCD_to_Dec((dr >> 8) & 0x1F);
    *date = BCD_to_Dec(dr & 0x3F);
    *weekday = (dr >> 13) & 0x07;
}

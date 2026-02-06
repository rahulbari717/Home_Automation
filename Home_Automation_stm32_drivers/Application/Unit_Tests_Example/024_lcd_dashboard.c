/*
 * 024_lcd_dashboard.c
 *
 * Description: Real-time Dashboard showing Time, Date, LDR Value, and LED Status.
 * Hardware: LCD (Port C), LDR (PA0/PA1), LED (PA5)
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 */

#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
#include "config.h"
#include "bsp_lcd.h"
#include "bsp_adc_sensors.h"
#include "stm32f446xx_rtc_driver.h"
#include "stm32f446xx_timer_driver.h"
#include "stm32f446xx_gpio_driver.h"

// --- Threshold Config ---
#define LDR_THRESHOLD   2000  // Adjust this based on your room light

// --- Global Handles ---
RTC_Handle_t rtc_handle;

// --- Helper Functions ---
void System_Init_All(void);
void LED_Control_Init(void);
void RTC_Hardcode_Setup(void);

int main(void)
{
    // 1. Initialize Drivers
    System_Init_All();

    // 2. Variables
    uint8_t h, m, s;
    uint8_t day, month, year, weekday;
    uint16_t ldr_val;
    char line1_buffer[17]; // 16 chars + null
    char line2_buffer[17];
    char *led_status_str;

    // 3. Splash Screen
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);
    BSP_LCD_PrintString("SYSTEM DASHBOARD");
    TIMER_DelayMs(TIM2, 2000);
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);

    while(1)
    {
        // --- STEP 1: READ SENSORS ---
        RTC_GetTime(&rtc_handle, &h, &m, &s);
        RTC_GetDate(&rtc_handle, &year, &month, &day, &weekday);
        ldr_val = BSP_Sensor_ReadLDR();

        // --- STEP 2: LOGIC CONTROL ---
        // Dark Room = High LDR Value (usually) -> Turn LED ON
        if(ldr_val > LDR_THRESHOLD) {
            GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, 1); // LED ON
            led_status_str = "ON ";
        } else {
            GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, 0); // LED OFF
            led_status_str = "OFF";
        }

        // --- STEP 3: UPDATE LCD ---

        // Line 1: Time & Date (Format: HH:MM DD/MM)
        // Note: %02d ensures "5" becomes "05"
        sprintf(line1_buffer, "%02d:%02d:%02d %02d/%02d", h, m, s, day, month);
        BSP_LCD_SetCursor(0, 0);
        BSP_LCD_PrintString(line1_buffer);

        // Line 2: LDR & LED (Format: LDR:1234 L:ON)
        sprintf(line2_buffer, "LDR:%4d L:%s", ldr_val, led_status_str);
        BSP_LCD_SetCursor(1, 0);
        BSP_LCD_PrintString(line2_buffer);

        // --- STEP 4: DELAY ---
        TIMER_DelayMs(TIM2, 500); // Update every half second
    }
}

// ============================================================================
// INITIALIZATION HELPERS
// ============================================================================

void System_Init_All(void)
{
    TIMER_DelayInit();      // Timer 2 for delays
    BSP_LCD_Init();         // LCD 16x2
    BSP_Sensors_Init();     // ADC for LDR
    LED_Control_Init();     // LED GPIO
    RTC_Hardcode_Setup();   // RTC Clock
}

void LED_Control_Init(void)
{
    GPIO_Handle_t led;
    led.pGPIOx = LED_PORT; // Usually GPIOA
    led.GPIO_PinConfig.GPIO_PinNumber = LED_GREEN_PIN; // PA5
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&led);
}

void RTC_Hardcode_Setup(void)
{
    // Hardcoding a start time so you don't need UART to type it every time
    rtc_handle.pRTC = RTC;
    RTC_Init(&rtc_handle);

    // Set to: 10:30:00 AM, 24th Jan 2026
    RTC_SetTime(&rtc_handle, 10, 30, 00);
    RTC_SetDate(&rtc_handle, 26, 1, 24, 6); // Year, Month, Day, Weekday
}

/*
 * 025_lcd_test.c
 *
 * Description: Verifies the 16x2 LCD Driver Integration
 * Goal: Print "Hello Rahul ! " on Line 1 and "SYSTEM READY" on Line 2
 */

#include "stm32f446xx.h"
#include "bsp_lcd.h"
#include "stm32f446xx_timer_driver.h"

int main(void)
{
    // 1. Initialize Drivers
    TIMER_DelayInit();
    BSP_LCD_Init();

    while(1)
    {
        // Test 1: Static Text
        BSP_LCD_SendCommand(LCD_CMD_CLEAR);

        BSP_LCD_SetCursor(0, 2); // Row 0, Col 2 (Center-ish)
        BSP_LCD_PrintString("Hello Rahul !");

        BSP_LCD_SetCursor(1, 0); // Row 1, Col 0
        BSP_LCD_PrintString("SYSTEM READY");

        TIMER_DelayMs(TIM2, 3000); // Wait 3 seconds

        // Test 2: Animation Effect (Typing)
        BSP_LCD_SendCommand(LCD_CMD_CLEAR);
        BSP_LCD_SetCursor(0, 0);

        char *msg = "Initializing...";
        while(*msg) {
            BSP_LCD_SendData(*msg++);
            TIMER_DelayMs(TIM2, 100); // Typing effect
        }

        TIMER_DelayMs(TIM2, 2000);
    }
}

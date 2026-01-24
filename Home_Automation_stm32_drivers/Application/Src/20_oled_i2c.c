/*
 * 20_oled_i2c.c
 *
 * Description: Tests the SSD1306 OLED via I2C1 (PB6/PB7).
 * Displays a pattern to verify the driver.
 *
 * Created on: Jan 23, 2026
 * Author: Rahul B.
 */

#include "stm32f446xx.h"
#include "bsp_i2c_oled.h"
#include "stm32f446xx_timer_driver.h"

int main(void)
{
    // 1. Initialize Timers for Delay
    TIMER_DelayInit();

    // 2. Initialize OLED (Also inits I2C1)
    BSP_OLED_Init();

    // 3. Main Loop
    while(1)
    {
        // --- Frame 1: Clear Screen ---
        BSP_OLED_Clear();
        BSP_OLED_Update();
        TIMER_DelayMs(TIM2, 1000);

        // --- Frame 2: Draw Border & Pixel ---
        // Draw a border around the screen
        for(int x=0; x<128; x++) {
            BSP_OLED_DrawPixel(x, 0, 1);    // Top Line
            BSP_OLED_DrawPixel(x, 63, 1);   // Bottom Line
        }
        for(int y=0; y<64; y++) {
            BSP_OLED_DrawPixel(0, y, 1);    // Left Line
            BSP_OLED_DrawPixel(127, y, 1);  // Right Line
        }

        // Draw a cross in the middle
        for(int i=0; i<64; i++) {
            BSP_OLED_DrawPixel(64, i, 1);
            BSP_OLED_DrawPixel(32+i, 32, 1);
        }

        BSP_OLED_Update();
        TIMER_DelayMs(TIM2, 2000);

        // --- Frame 3: Checkerboard Pattern ---
        BSP_OLED_Clear();
        for(int y=0; y<64; y+=2) {
            for(int x=0; x<128; x+=2) {
                BSP_OLED_DrawPixel(x, y, 1);
            }
        }
        BSP_OLED_Update();
        TIMER_DelayMs(TIM2, 2000);
    }
}

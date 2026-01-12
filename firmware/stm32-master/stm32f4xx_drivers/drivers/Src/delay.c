/*
 * delay.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "buzzer.h"
#include "stm32f446xx_gpio_driver.h"

// --- SysTick Register Definitions (Core Peripheral) ---
#define SYSTICK_BASE        0xE000E010U
#define SYSTICK_CTRL        (*(volatile uint32_t*)(SYSTICK_BASE + 0x00))
#define SYSTICK_LOAD        (*(volatile uint32_t*)(SYSTICK_BASE + 0x04))
#define SYSTICK_VAL         (*(volatile uint32_t*)(SYSTICK_BASE + 0x08))

#define CTRL_ENABLE         (1 << 0)
#define CTRL_CLKSRC         (1 << 2) // Indicates use of Processor Clock (16MHz)
#define CTRL_COUNTFLAG      (1 << 16)

// --- Precise Delay Function ---
// accurate to +/- 1% (limit of HSI oscillator)
void delay_ms(uint32_t ms)
{
    // 1. Calculate the Load Value for exactly 1 Millisecond
    // Formula: SystemClock / 1000
    // Default HSI is 16,000,000 Hz.
    // 16,000,000 / 1000 = 16,000 ticks per ms.
    uint32_t one_ms_ticks = 16000;

    // 2. Configure SysTick
    SYSTICK_CTRL = 0;             // Disable SysTick during setup
    SYSTICK_LOAD = one_ms_ticks - 1; // Set Reload Register
    SYSTICK_VAL  = 0;             // Clear Current Value
    SYSTICK_CTRL = CTRL_ENABLE | CTRL_CLKSRC; // Enable with Processor Clock

    // 3. Loop 'ms' times
    for(uint32_t i = 0; i < ms; i++)
    {
        // Wait until COUNTFLAG is set (Timer reached 0 and reloaded)
        while((SYSTICK_CTRL & CTRL_COUNTFLAG) == 0);
    }

    // 4. Disable SysTick
    SYSTICK_CTRL = 0;
}

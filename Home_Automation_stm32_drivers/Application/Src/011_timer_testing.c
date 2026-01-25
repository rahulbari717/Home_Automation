/*
 * 011_timer_testing.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 */

#include "bsp_init.h"

USART_Handle_t usart2_handle;

int main(void)
{
	// 1. CRITICAL: Configure Clock to 180MHz FIRST
	// This switches from the inaccurate HSI to the accurate HSE Crystal.
	SystemClock_Config_HSE_180MHz();
    // 1. Initialize delay timer (call ONCE at startup)
	app_init();
    TIMER_DelayInit();

    // 2. Now you can use delays anywhere!
    while(1){
    	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    	// Delay 1 sec means 1000 milliseconds
    	TIMER_DelayMs(TIM2, 1000);
    }
}

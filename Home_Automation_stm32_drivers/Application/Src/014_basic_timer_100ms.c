/*
 * 014_basic_timer_100ms.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul B.
 * Description : PA5 LED 100ms delay using Basic Timer 6
 */

#include "bsp_init.h"

USART_Handle_t usart2_handle;

int main(void)
{
    // 1. Initialize GPIO
    app_init();

    while(1){
		// Delay 100ms
		// This calculates its own math based on current clock
		TIMER_Basic_DelayMs(TIM6, 100);

		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
	}
}

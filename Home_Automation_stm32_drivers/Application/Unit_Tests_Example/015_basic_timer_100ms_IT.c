/*
 * 015_basic_timer_100ms_IT.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 */

#include "bsp_init.h"

TIMER_Handle_t htimer7; // Global handle
USART_Handle_t usart2_handle;

volatile uint8_t g_TimerDelayComplete = 0;

int main(void)
{
    app_init();

    // Set up the handle so the ISR can use it
    htimer7.pTIMx = TIM7;

    while(1){
        TIMER_Basic_DelayMs_IT(TIM7, 500);
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    }
}

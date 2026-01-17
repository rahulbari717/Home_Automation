/*
 * 015_basic_timer_100ms_IT.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 */

#include "stm32f446xx.h"

TIMER_Handle_t htimer7; // Global handle
volatile uint8_t g_TimerDelayComplete = 0;

void gpio_init(){
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}

int main(void)
{
    gpio_init();

    // Set up the handle so the ISR can use it
    htimer7.pTIMx = TIM7;

    while(1){
        TIMER_Basic_DelayMs_IT(TIM7, 500);
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    }
}

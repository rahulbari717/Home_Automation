/*
 * 014_basic_timer_100ms.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul B.
 * Description : PA5 LED 100ms delay using Basic Timer 6
 */

#include "stm32f446xx.h"

void TIMER6_Init(void);

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
    // 1. Initialize GPIO
    gpio_init();

    while(1){
		// Delay 100ms
		// This calculates its own math based on current clock
		TIMER_Basic_DelayMs(TIM6, 100);

		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
	}
}

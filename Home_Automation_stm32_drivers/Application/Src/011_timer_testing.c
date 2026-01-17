/*
 * 011_timer_testing.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 */

#include "stm32f446xx.h"
#include "stm32f446xx_timer_driver.h"

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
	// 1. CRITICAL: Configure Clock to 180MHz FIRST
	// This switches from the inaccurate HSI to the accurate HSE Crystal.
	SystemClock_Config_HSE_180MHz();
    // 1. Initialize delay timer (call ONCE at startup)
    TIMER_DelayInit();
    gpio_init();

    // 2. Now you can use delays anywhere!
    while(1){
    	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    	// Delay 1 sec means 1000 milliseconds
    	TIMER_DelayMs(TIM2, 1000);
    }
}

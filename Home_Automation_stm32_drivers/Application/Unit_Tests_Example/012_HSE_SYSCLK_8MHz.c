/*
 * 012_HSE_SYSCLK_8MHz.c
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 *  Description: Sets system to 8MHz (HSE) using clean driver API
 */

#include "stm32f446xx.h"

void gpio_init(void)
{
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}

int main(void)
{
    // 1. Configure System Clock to 8MHz HSE (using BYPASS mode for Nucleo)
    //    This completely replaces your SystemClock_Config_HSE_8MHz() function
    RCC_SetSystemClock_HSE(HSE_CLK_VALUE, RCC_HSE_BYPASS);

    // 2. Initialize Timer (Will automatically detect 8MHz clock)
    TIMER_DelayInit();

    // 3. Initialize GPIO
    gpio_init();

    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 1000);
    }
}

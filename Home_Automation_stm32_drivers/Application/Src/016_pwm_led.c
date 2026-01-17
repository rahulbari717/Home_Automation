/*
 * 016_pwm_led.c
 * Author: Rahul B.
 * Description : Fade LED on PA5 using TIM2 Channel 1 PWM
 */

#include "stm32f446xx.h"

TIMER_Handle_t htimer2;

void gpio_init(void)
{
    // Configure PA5 as Timer 2 Channel 1 (Alternate Function 1)
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;

    // IMPORTANT: Mode must be Alternate Function, NOT Output
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    GpioLed.GPIO_PinConfig.GPIO_PinAltFunMode = 1; // AF1 is TIM1/TIM2 for PA5

    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_Init(&GpioLed);
}

void TIMER2_Init(void)
{
    htimer2.pTIMx = TIM2;

    // 1. Timer Base Configuration
    // We want a PWM frequency of 1 kHz (Good for LEDs)
    // Clock = 16 MHz (HSI).
    // Let's set Prescaler to 15 (16MHz / 16 = 1 MHz Timer Clock)
    htimer2.TIMER_Config.TIMER_Prescaler = 15;

    // Period (ARR) = (TimerClock / PWM_Freq) - 1
    // (1,000,000 / 1000) - 1 = 999.
    // So timer counts 0 to 999.
    htimer2.TIMER_Config.TIMER_Period = 999;

    TIMER_PeriClockControl(TIM2, ENABLE);
    TIMER_BaseInit(&htimer2);

    // 2. PWM Configuration
    TIMER_OC_Config_t timer2_pwm_config;

    timer2_pwm_config.TIMER_OCMode = TIMER_OC_MODE_PWM1;   // PWM Mode 1
    timer2_pwm_config.TIMER_OCPolarity = TIMER_OC_POL_HIGH; // Active High
    timer2_pwm_config.TIMER_Pulse = 0; // Start with 0% duty cycle (LED OFF)

    // Configure Channel 1 (PA5)
    TIMER_PWM_Config(TIM2, TIMER_CHANNEL_1, &timer2_pwm_config);

    // 3. Start PWM
    TIMER_PWM_Start(TIM2, TIMER_CHANNEL_1);
}

void delay_simple(uint32_t count)
{
    for(uint32_t i = 0; i < count; i++);
}

int main(void)
{
    gpio_init();
    TIMER2_Init();

    uint16_t brightness = 0;
    uint8_t direction = 1; // 1 = Fading IN, 0 = Fading OUT

    while(1)
    {
        // Set the Duty Cycle (CCR1 register)
        TIMER_PWM_SetDutyCycle(TIM2, TIMER_CHANNEL_1, brightness);

        // Simple delay to make the fading visible to human eye
        delay_simple(2000);

        // Update brightness logic
        if(direction == 1)
        {
            brightness += 10; // Increase brightness
            if(brightness >= htimer2.TIMER_Config.TIMER_Period) // Max reached?
            {
                direction = 0; // Start fading out
                brightness = htimer2.TIMER_Config.TIMER_Period;
            }
        }
        else
        {
            brightness -= 10; // Decrease brightness
            if(brightness <= 0) // Min reached?
            {
                direction = 1; // Start fading in
                brightness = 0;
            }
        }
    }
}

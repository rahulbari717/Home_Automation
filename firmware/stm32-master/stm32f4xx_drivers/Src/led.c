/*
 * led.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "led.h"
#include "stm32f446xx_gpio_driver.h"  // ← MUST INCLUDE
#include "config.h"                    // ← MUST INCLUDE
#include "lcd.h"                       // For LCD_DelayMs

extern void delay_ms(uint32_t ms);

void LED_Init(void)
{
    GPIO_Handle_t led;

    GPIO_PeriClockControl(LED_PORT, ENABLE);

    led.pGPIOx = LED_PORT;
    led.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    led.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // Green LED
    led.GPIO_PinConfig.GPIO_PinNumber = LED_GREEN_PIN;
    GPIO_Init(&led);
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);

    // Red LED
    led.GPIO_PinConfig.GPIO_PinNumber = LED_RED_PIN;
    GPIO_Init(&led);
    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
}

void LED_GreenOn(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_SET);
}

void LED_GreenOff(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
}

void LED_RedOn(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_SET);
}

void LED_RedOff(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
}

void LED_Flash(GPIO_RegDef_t *pGPIOx, uint8_t pin, uint8_t count, uint16_t duration_ms)
{
    for(uint8_t i = 0; i < count; i++)
    {
        // Turn ON using the passed Port pointer
        GPIO_WriteToOutputPin(pGPIOx, pin, GPIO_PIN_SET);
        delay_ms(duration_ms);

        // Turn OFF
        GPIO_WriteToOutputPin(pGPIOx, pin, GPIO_PIN_RESET);

        // Delay between flashes (only if there is another flash coming)
        if(i < count - 1) {
            delay_ms(duration_ms);
        }
    }
}

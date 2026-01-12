/*
 * button.c
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#include "stm32f446xx.h"  // For GPIO_Handle_t, GPIO functions
#include "config.h"                    // For pin definitions (WAKEUP_BTN_PORT, etc.)
#include <stdint.h>                    // For uint8_t, uint16_t
#include "button.h"
#include <string.h>

extern void delay_ms(uint32_t ms);

void WakeupButton_Init(void)
{
    GPIO_Handle_t button;
    memset(&button, 0, sizeof(button));

    button.pGPIOx = WAKEUP_BTN_PORT;
    button.GPIO_PinConfig.GPIO_PinNumber = WAKEUP_BTN_PIN;
    button.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
    button.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    button.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(WAKEUP_BTN_PORT, ENABLE);

    GPIO_Init(&button);

    GPIO_IRQPriorityConfig(WAKEUP_BTN_IRQ, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(WAKEUP_BTN_IRQ, ENABLE);
}

uint8_t WakeupButton_IsPressed(void)
{
    return (GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == GPIO_PIN_RESET);
}

// IRQ Handler for EXTI15_10 (handles PC13)
void EXTI15_10_IRQHandler(void){
	delay(100); // Simple debounce
	GPIO_IRQHandling(WAKEUP_BTN_PIN); // Clear the pending bit
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5); // Toggle LED
}


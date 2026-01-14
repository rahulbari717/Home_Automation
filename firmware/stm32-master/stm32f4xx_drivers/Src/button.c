///*
// * button.c
// *
// *  Created on: Jan 12, 2026
// *      Author: Rahul B.
// */
//
//#include "stm32f446xx.h"  // For GPIO_Handle_t, GPIO functions
//#include "config.h"                    // For pin definitions (WAKEUP_BTN_PORT, etc.)
//#include <stdint.h>                    // For uint8_t, uint16_t
//#include "button.h"
//#include <string.h>
//
//extern void delay_ms(uint32_t ms);
//
//void WakeupButton_Init(void)
//{
//    GPIO_Handle_t button;
//    memset(&button, 0, sizeof(button));
//
//    button.pGPIOx = WAKEUP_BTN_PORT;
//    button.GPIO_PinConfig.GPIO_PinNumber = WAKEUP_BTN_PIN;
//    button.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
//    button.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
//    button.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
//
//    GPIO_PeriClockControl(WAKEUP_BTN_PORT, ENABLE);
//
//    GPIO_Init(&button);
//
//    GPIO_IRQPriorityConfig(WAKEUP_BTN_IRQ, NVIC_IRQ_PRI15);
//    GPIO_IRQInterruptConfig(WAKEUP_BTN_IRQ, ENABLE);
//}
//
//uint8_t WakeupButton_IsPressed(void)
//{
//    return (GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == GPIO_PIN_RESET);
//}
//
//// IRQ Handler for EXTI15_10 (handles PC13)
////void EXTI15_10_IRQHandler(void){
////	delay(100); // Simple debounce
////	GPIO_IRQHandling(WAKEUP_BTN_PIN); // Clear the pending bit
////	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5); // Toggle LED
////}
//

//---------------------------------------------------------------------------------------------
/*
 * Src/button.c
 * Fixed: Added missing includes for GPIO and Config
 */

#include "button.h"
#include "stm32f446xx.h"              // <--- REQUIRED for RCC, SYSCFG
#include "stm32f446xx_gpio_driver.h"  // <--- REQUIRED for GPIO_Handle_t
#include "config.h"                   // <--- REQUIRED for WAKEUP_BTN_PORT
#include <string.h>

// Debug print helper
extern void UART_Printf(const char *format, ...);

void WakeupButton_Init(void)
{
    UART_Printf("   >> [BTN] Configuring GPIO PC13...\r\n");

    GPIO_Handle_t button;
    memset(&button, 0, sizeof(button));

    // These macros (WAKEUP_BTN_PORT) come from config.h
    button.pGPIOx = WAKEUP_BTN_PORT;
    button.GPIO_PinConfig.GPIO_PinNumber = WAKEUP_BTN_PIN;

    // Interrupt on Falling Edge (Button Press)
    button.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_FT;
    button.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    button.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    GPIO_PeriClockControl(WAKEUP_BTN_PORT, ENABLE);
    GPIO_Init(&button);

    // --- SYSCFG FIX FOR INTERRUPTS ---
    UART_Printf("   >> [BTN] Enabling SYSCFG and Routing EXTI13 to Port C...\r\n");

    // 1. Enable SYSCFG Clock
    RCC->APB2ENR |= (1 << 14);

    // 2. Select Port C for EXTI13 (Bits 4-7 of EXTICR[3])
    // EXTICR[0] = Pins 0-3, EXTICR[1] = Pins 4-7, EXTICR[2] = Pins 8-11, EXTICR[3] = Pins 12-15
    SYSCFG->EXTICR[3] &= ~(0xF << 4); // Clear 4 bits at position 4
    SYSCFG->EXTICR[3] |= (0x2 << 4);  // Set value 0x2 (Port C) at position 4

    UART_Printf("   >> [BTN] Enabling NVIC IRQ...\r\n");

    // These macros come from gpio_driver.h
    GPIO_IRQPriorityConfig(WAKEUP_BTN_IRQ, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(WAKEUP_BTN_IRQ, ENABLE);

    UART_Printf("   >> [BTN] Init Complete.\r\n");
}

uint8_t WakeupButton_IsPressed(void)
{
    return (GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN) == 0);
}

/*
 * bsp_button.c
 *
 *  Created on: Jan 26, 2026
 *      Author: Rahul B.
 */

#include "bsp_button.h"

/**
 * @brief  Initializes the Wakeup button (PC13) with Interrupt capability.
 */
void BSP_Button_Init(void) {
    GPIO_Handle_t btn;

    // Enable clock for the Button Port (Port C)
    GPIOC_PCLK_EN();

    btn.pGPIOx = WAKEUP_BTN_PORT;
    btn.GPIO_PinConfig.GPIO_PinNumber = WAKEUP_BTN_PIN;

    // Using Rising/Falling edge trigger as per your 004_buttonInterruptRFT.c
    btn.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;
    btn.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    btn.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; // External pull-up usually exists

    GPIO_Init(&btn);

    // Configure NVIC for EXTI Line 15_10
    GPIO_IRQPriorityConfig(WAKEUP_BTN_IRQ, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(WAKEUP_BTN_IRQ, ENABLE);
}

/**
 * @brief  Reads the current physical state of the button.
 * @return BSP_BUTTON_PRESSED or BSP_BUTTON_RELEASED
 */
uint8_t BSP_Button_GetState(void) {
    return GPIO_ReadFromInputPin(WAKEUP_BTN_PORT, WAKEUP_BTN_PIN);
}

/**
 * @brief  Weak callback function that the user can override in main.c
 */
__weak void BSP_Button_Callback(void) {
    // Note: User should implement this in main.c to handle the event
}

/**
 * @brief  Handles the specific IRQ logic for the button.
 * Call this from the EXTI15_10_IRQHandler in main.c.
 */
void BSP_Button_IRQHandling(void) {
    // Debounce delay (Simple method from your reference)
    for(int i = 0; i < 500000/2; i++);

    // Clear the pending interrupt flag in the GPIO driver
    GPIO_IRQHandling(WAKEUP_BTN_PIN);

    // Execute the user callback
    BSP_Button_Callback();
}

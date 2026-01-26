/*
 * bsp_buzzer.c
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#include "bsp_buzzer.h"
#include "bsp_delay.h"

/* ===== Buzzer Initialization ===== */
void BSP_Buzzer_Init(void) { // Added missing initialization function
    GPIO_Handle_t buzzer_pin;
    buzzer_pin.pGPIOx = BUZZER_PORT;
    buzzer_pin.GPIO_PinConfig.GPIO_PinNumber = BUZZER_PIN;
    buzzer_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    buzzer_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    buzzer_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    buzzer_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&buzzer_pin);
}
/* ================= BUZZER CONTROL FUNCTIONS ================= */

/**
 * @brief Turn ON the buzzer
 *
 * Usage: BSP_Buzzer_On();
 */
void BSP_Buzzer_On(void)
{
    GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

/**
 * @brief Turn OFF the buzzer
 *
 * Usage: BSP_Buzzer_Off();
 */
void BSP_Buzzer_Off(void)
{
    GPIO_WriteToOutputPin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
}

/**
 * @brief Toggle the buzzer state
 *
 * Usage: BSP_Buzzer_Toggle();
 */
void BSP_Buzzer_Toggle(void)
{
    GPIO_ToggleOutputPin(BUZZER_PORT, BUZZER_PIN);
    BSP_Delay_1s();
}

/**
 * @brief Sound buzzer for a specific duration
 * @param duration_ms: Duration in milliseconds
 *
 * Usage: BSP_Buzzer_Beep(200); // Beep for 200ms
 */
void BSP_Buzzer_Beep(uint32_t duration_ms)
{
    BSP_Buzzer_On();
    BSP_Delay_ms(1000);
    BSP_Buzzer_Off();
}

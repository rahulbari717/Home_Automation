/*
 * bsp_led.c
 *
 *  Created on: Jan 26, 2026
 *      Author: Rahul B.
 */

#include "bsp_led.h"

/* ===== LED Initialization ===== */
void BSP_LED_Init(void){
    GPIO_Handle_t led_pin;

    led_pin.pGPIOx = LED_PORT;
    led_pin.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    led_pin.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_LOW;
    led_pin.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    led_pin.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // Green LED
    led_pin.GPIO_PinConfig.GPIO_PinNumber = LED_GREEN_PIN;
    GPIO_Init(&led_pin);

    // Red LED
    led_pin.GPIO_PinConfig.GPIO_PinNumber = LED_RED_PIN;
    GPIO_Init(&led_pin);

    // White LED
    led_pin.GPIO_PinConfig.GPIO_PinNumber = LED_WHITE_PIN;
    GPIO_Init(&led_pin);
}

/* ================= LED CONTROL FUNCTIONS ================= */

/**
 * @brief Turn ON a specific LED
 * @param PinNumber: LED pin number (e.g., LED_GREEN_PIN, LED_RED_PIN, LED_WHITE_PIN)
 *
 * Usage: BSP_LED_On(LED_RED_PIN);
 */
void BSP_LED_On(uint8_t PinNumber)
{
    GPIO_WriteToOutputPin(LED_PORT, PinNumber, GPIO_PIN_SET);
}

/**
 * @brief Turn OFF a specific LED
 * @param PinNumber: LED pin number (e.g., LED_GREEN_PIN, LED_RED_PIN, LED_WHITE_PIN)
 *
 * Usage: BSP_LED_Off(LED_RED_PIN);
 */
void BSP_LED_Off(uint8_t PinNumber)
{
    GPIO_WriteToOutputPin(LED_PORT, PinNumber, GPIO_PIN_RESET);
}

/**
 * @brief Toggle a specific LED
 * @param PinNumber: LED pin number (e.g., LED_GREEN_PIN, LED_RED_PIN, LED_WHITE_PIN)
 *
 * Usage: BSP_LED_Toggle(LED_WHITE_PIN);
 */
void BSP_LED_Toggle(uint8_t PinNumber)
{
    GPIO_ToggleOutputPin(LED_PORT, PinNumber);
}

/**
 * @brief Turn OFF all LEDs
 *
 * Usage: BSP_LED_AllOff();
 */
void BSP_LED_AllOff(void)
{
    GPIO_WriteToOutputPin(LED_PORT, LED_GREEN_PIN, GPIO_PIN_RESET);
    GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, GPIO_PIN_RESET);
    GPIO_WriteToOutputPin(LED_PORT, LED_WHITE_PIN, GPIO_PIN_RESET);
}


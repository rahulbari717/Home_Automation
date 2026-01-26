/*
 * bsp_relay.c
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#include "bsp_relay.h"

/* ===== Relay Initialization ===== */
void Relay_Init(void) {
    GPIO_Handle_t relay_gpio;

    relay_gpio.pGPIOx = RELAY_PORT;
    relay_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    relay_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    relay_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    relay_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    const uint8_t relay_pins[4] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN};
    for(int i = 0; i < 4; i++) {
        relay_gpio.GPIO_PinConfig.GPIO_PinNumber = relay_pins[i];
        GPIO_Init(&relay_gpio);
        GPIO_WriteToOutputPin(RELAY_PORT, relay_pins[i], GPIO_PIN_RESET);
    }
}

void BSP_Relay_Init(void) {
    GPIO_Handle_t relay;
    relay.pGPIOx = RELAY_PORT;
    relay.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    relay.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    relay.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    relay.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;

    uint8_t pins[] = {RELAY1_PIN, RELAY2_PIN, RELAY3_PIN, RELAY4_PIN};
    for(int i = 0; i < 4; i++) {
        relay.GPIO_PinConfig.GPIO_PinNumber = pins[i];
        GPIO_Init(&relay);
        GPIO_WriteToOutputPin(RELAY_PORT, pins[i], GPIO_PIN_RESET);
    }
}

void BSP_Relay_SetState(uint8_t relay_pin, uint8_t state) {
    GPIO_WriteToOutputPin(RELAY_PORT, relay_pin, state);
}

void BSP_Relay_Toggle(uint8_t relay_pin) {
    GPIO_ToggleOutputPin(RELAY_PORT, relay_pin);
}

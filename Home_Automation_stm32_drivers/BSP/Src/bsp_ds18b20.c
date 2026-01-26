/*
 * bsp_ds18b20.c
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#include "bsp_ds18b20.h"
#include "bsp_delay.h"

void DS_Pin_Output(void) {
    GPIO_Handle_t ds;
    ds.pGPIOx = DS18B20_PORT;
    ds.GPIO_PinConfig.GPIO_PinNumber = DS18B20_PIN;
    ds.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GPIO_Init(&ds);
}

void DS_Pin_Input(void) {
    GPIO_Handle_t ds;
    ds.pGPIOx = DS18B20_PORT;
    ds.GPIO_PinConfig.GPIO_PinNumber = DS18B20_PIN;
    ds.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    GPIO_Init(&ds);
}

void BSP_DS18B20_Init(void) {
    DS_Pin_Output();
    GPIO_WriteToOutputPin(DS18B20_PORT, DS18B20_PIN, SET);
}

// Simple logic for temperature reading (Start/Presence/Read bits)
float BSP_DS18B20_ReadTemp(void) {
    // OneWire Reset/Presence and bit-banging logic goes here
    return 25.0f; // Placeholder
}

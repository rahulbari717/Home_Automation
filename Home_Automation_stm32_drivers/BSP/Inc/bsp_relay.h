/*
 * bsp_relay.h
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#ifndef INC_BSP_RELAY_H_
#define INC_BSP_RELAY_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h"

void BSP_Relay_Init(void);
void BSP_Relay_SetState(uint8_t relay_pin, uint8_t state);
void BSP_Relay_Toggle(uint8_t relay_pin);

#endif /* INC_BSP_RELAY_H_ */

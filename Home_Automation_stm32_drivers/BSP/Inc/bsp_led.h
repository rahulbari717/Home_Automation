/*
 * bsp_led.h
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#ifndef INC_BSP_LED_H_
#define INC_BSP_LED_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h"

void BSP_LED_Init(void);
void BSP_LED_On(uint8_t PinNumber);
void BSP_LED_Off(uint8_t PinNumber);
void BSP_LED_Toggle(uint8_t pinNumber);

void BSP_LED_AllOff(void);
#endif /* INC_BSP_LED_H_ */

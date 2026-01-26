/*
 * bsp_button.h
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#ifndef INC_BSP_BUTTON_H_
#define INC_BSP_BUTTON_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h"

/* Logical Button States */
#define BSP_BUTTON_PRESSED      0   // PC13 pulled to GND when pressed
#define BSP_BUTTON_RELEASED     1

/* Function Prototypes */
void BSP_Button_Init(void);
uint8_t BSP_Button_GetState(void);
void BSP_Button_IRQHandling(void);

/* Callback function (Weakly defined in .c, can be overridden in main.c) */
void BSP_Button_Callback(void);

#endif /* INC_BSP_BUTTON_H_ */

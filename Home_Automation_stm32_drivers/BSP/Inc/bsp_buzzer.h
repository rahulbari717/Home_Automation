/*
 * bsp_buzzer.h
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#ifndef INC_BSP_BUZZER_H_
#define INC_BSP_BUZZER_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h"

void BSP_Buzzer_Init(void);
void BSP_Buzzer_On(void);
void BSP_Buzzer_Off(void);
void BSP_Buzzer_Toggle(void);

void BSP_Buzzer_Beep(uint32_t duration_ms);

#endif /* INC_BSP_BUZZER_H_ */

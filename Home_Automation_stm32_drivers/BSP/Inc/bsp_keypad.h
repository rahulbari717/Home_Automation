/*
 * stm32f446xx_keypad.h
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari
 */

#ifndef INC_STM32F446XX_KEYPAD_H_
#define INC_STM32F446XX_KEYPAD_H_

#include "stm32f446xx.h"
#include <stdint.h>

#define KEYPAD_NO_KEY   '\0'

/* ===== Function Prototypes ===== */
void Keypad_Init(void);
char Keypad_GetKey(void);
void Keypad_Delay(uint32_t delay);

#endif /* INC_STM32F446XX_KEYPAD_H_ */
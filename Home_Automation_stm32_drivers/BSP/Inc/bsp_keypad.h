/*
 * stm32f446xx_keypad.h
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari
 */

#ifndef INC_STM32F446XX_KEYPAD_H_
#define INC_STM32F446XX_KEYPAD_H_

#include "stm32f446xx.h"

/* ===== KEYPAD PIN CONFIGURATION ===== */
// Rows (Output) - Port B
#define KEYPAD_ROW_PORT         GPIOB
#define KEYPAD_R0_PIN           GPIO_PIN_NO_0
#define KEYPAD_R1_PIN           GPIO_PIN_NO_1
#define KEYPAD_R2_PIN           GPIO_PIN_NO_2
#define KEYPAD_R3_PIN           GPIO_PIN_NO_3

// Columns (Input with Pull-up) - Port B
#define KEYPAD_COL_PORT         GPIOB
#define KEYPAD_C0_PIN           GPIO_PIN_NO_4
#define KEYPAD_C1_PIN           GPIO_PIN_NO_5
#define KEYPAD_C2_PIN           GPIO_PIN_NO_6
#define KEYPAD_C3_PIN           GPIO_PIN_NO_7

/* ===== 4x4 Matrix Keypad Layout =====
 *        C0    C1    C2    C3
 *   R0   1     2     3     A
 *   R1   4     5     6     B
 *   R2   7     8     9     C
 *   R3   *     0     #     D
 */

#define KEYPAD_NO_KEY           0xFF

/* ===== Function Prototypes ===== */
void Keypad_Init(void);
char Keypad_GetKey(void);
void Keypad_Delay(uint32_t delay);

#endif /* INC_STM32F446XX_KEYPAD_H_ */
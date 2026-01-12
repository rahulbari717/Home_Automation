/*
 * keypad.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 *  Description: 4x4 Matrix Keypad driver
 */

#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include <stdint.h>
#include "config.h"

/* ===== KEYPAD LAYOUT ===== */
/*
 *        C0    C1    C2    C3
 *   R0   1     2     3     A
 *   R1   4     5     6     B
 *   R2   7     8     9     C
 *   R3   *     0     #     D
 */

/* ===== API FUNCTIONS ===== */

/**
 * @brief Initialize keypad GPIO pins
 */
void Keypad_Init(void);

/**
 * @brief Scan keypad and return pressed key
 * @return Character of pressed key, or '\0' if no key pressed
 */
char Keypad_GetKey(void);

/**
 * @brief Wait for a key press and return it (blocking)
 * @return Character of pressed key
 */
char Keypad_WaitForKey(void);

/**
 * @brief Get numeric input (for PIN entry)
 * @param buffer Buffer to store digits
 * @param max_length Maximum number of digits
 * @param mask_display If 1, display '*' instead of actual digits
 * @return Number of digits entered
 */
uint8_t Keypad_GetNumericInput(char *buffer, uint8_t max_length, uint8_t mask_display);

/**
 * @brief Debounce delay
 */
void Keypad_Debounce(void);

#endif /* INC_KEYPAD_H_ */

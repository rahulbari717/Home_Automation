/*
 * button.h
 *
 *  Created on: Jan 12, 2026
 *      Author: lenovo
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include <stdint.h>

void WakeupButton_Init(void);
uint8_t WakeupButton_IsPressed(void);

#endif

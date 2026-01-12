/*
 * buzzer.h
 *
 *  Created on: Jan 12, 2026
 *      Author: lenovo
 */

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

#include <stdint.h>

void Buzzer_Init(void);
void Buzzer_Beep(uint8_t count, uint16_t duration_ms);
void Buzzer_On(void);
void Buzzer_Off(void);
void delay_ms(uint32_t ms);

#endif

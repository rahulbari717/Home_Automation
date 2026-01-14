/*
 * main.h
 *
 *  Created on: Jan 12, 2026
 *      Author: Rahul B.
 */

#ifndef MAIN_H_
#define MAIN_H_

/* ===== FUNCTION PROTOTYPES ===== */
void System_Init(void);
void State_DeepSleep(void);
void State_Authentication(void);
void State_MenuNavigation(void);
void State_Lockout(void);
void Hardware_Init(void);
void LED_Init(void);
void Buzzer_Init(void);
void WakeupButton_Init(void);
void Buzzer_Beep(uint8_t count, uint16_t duration_ms);

// --- Function Prototypes (Tells compiler these exist) ---
void USART2_GPIOInit(void);
void USART2_Init(void);

#endif /* MAIN_H_ */

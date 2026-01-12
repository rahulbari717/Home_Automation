/*
 * led.h
 *
 *  Created on: Jan 12, 2026
 *      Author: lenovo
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include <stdint.h>

void LED_Init(void);
void LED_GreenOn(void);
void LED_GreenOff(void);
void LED_RedOn(void);
void LED_RedOff(void);
void LED_Flash(uint8_t pin, uint8_t count, uint16_t duration_ms);

#endif

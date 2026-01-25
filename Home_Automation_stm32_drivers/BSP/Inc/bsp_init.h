/*
 * bsp_init.h
 *
 *  Created on: Jan 24, 2026
 *      Author: Rahul B.
 */

#ifndef INC_BSP_INIT_H_
#define INC_BSP_INIT_H_

#include "main.h"
#include "config.h"

extern USART_Handle_t usart2_handle;

void app_init(void);
void gpio_init(void);
void button_init(void);
void USART2_GPIOInit(void);
void USART2_Init(void);
void UART_Printf(const char *format, ...);

#endif /* INC_BSP_INIT_H_ */

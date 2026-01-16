/*
 * bsp_debug.h
 * Author: Rahul Bari
 * Purpose: Common Debug Console (UART2) functions
 */

#ifndef INC_BSP_UART2_DEBUG_H_
#define INC_BSP_UART2_DEBUG_H_

#include "stm32f446xx.h"
#include <stdint.h>
#include <stdarg.h>

/* Initializes GPIO PA2/PA3 and USART2 @ 115200 */
void Debug_Init(void);

/* printf style function for UART */
void UART_Printf(const char *format, ...);

/* Blocking receive 1 byte */
uint8_t UART_ReceiveByte(void);

/* Clears terminal screen */
void Debug_ClearScreen(void);

void Trigger_DivideByZero(void);
void Trigger_UnalignedAccess(void);
void Trigger_InvalidInstruction(void);
void Trigger_BusFault(void);


#endif /* BSP_DEBUG_H_ */


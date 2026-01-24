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
#include <stdio.h>   // for vsnprintf
#include <string.h> // for strlen

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

/* ... other prototypes ... */

/* NEW: Function to get the UART Handle */
USART_Handle_t* Debug_GetHandle(void);

#endif /* BSP_UART2_DEBUG_H_ */



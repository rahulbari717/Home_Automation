/*
 * bsp_bluetooth.h
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: BSP for HC-05 Bluetooth Module
 */

#ifndef INC_BSP_BLUETOOTH_H_
#define INC_BSP_BLUETOOTH_H_

#include "stm32f446xx.h"
#include <stdint.h>

/* Initialize UART1 for HC-05 @ 9600 baud */
void BSP_Bluetooth_Init(void);

/* Send a string via Bluetooth */
void BSP_Bluetooth_SendString(const char *str);

/* Send formatted string (printf style) */
void BSP_Bluetooth_Printf(const char *format, ...);

/* Receive one byte (blocking) */
uint8_t BSP_Bluetooth_ReceiveByte(void);

/* Check if data available (non-blocking) */
uint8_t BSP_Bluetooth_DataAvailable(void);

/* Send AT command for configuration */
void BSP_Bluetooth_SendATCommand(const char *cmd);

#endif /* INC_BSP_BLUETOOTH_H_ */
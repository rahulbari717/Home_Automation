/*
 * bsp_bluetooth.h
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: BSP for HC-05 Bluetooth Module
 */

#ifndef INC_BSP_BLUETOOTH_H_
#define INC_BSP_BLUETOOTH_H_

#include <stdint.h>

void BSP_Bluetooth_Init(void);
void BSP_Bluetooth_SendString(const char *str);
void BSP_Bluetooth_Printf(const char *format, ...);
uint8_t BSP_Bluetooth_ReceiveByte(void);
uint8_t BSP_Bluetooth_DataAvailable(void);
#endif /* INC_BSP_BLUETOOTH_H_ */

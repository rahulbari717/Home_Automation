/*
 * fault_handler.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Rahul Bari.
 */

#ifndef INC_STM32F446XX_FAULT_HANDLER_H_
#define INC_STM32F446XX_FAULT_HANDLER_H_

void Fault_Init(void);
void Fault_Enable_Traps(void);
void HardFault_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
#endif /* INC_STM32F446XX_FAULT_HANDLER_H_ */

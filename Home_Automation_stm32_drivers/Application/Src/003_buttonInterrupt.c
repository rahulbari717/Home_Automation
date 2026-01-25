/*
 * 003_buttonInterrupt.c
 *
 *  Created on: Nov 25, 2025
 *  Author: Rahul B.
 *  Description : when button pressed led on another time press led off
 *
 */

#include "bsp_init.h"

void delay(void){
	for(int i=0; i<500000/2; i++);
}

int main(){
	app_init();
	while(1);
}

// IRQ Handler for EXTI15_10 (handles PC13)
void EXTI15_10_IRQHandler(void){
	delay(); // Simple debounce
	GPIO_IRQHandling(GPIO_PIN_NO_13); // Clear the pending bit
	GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5); // Toggle LED
}

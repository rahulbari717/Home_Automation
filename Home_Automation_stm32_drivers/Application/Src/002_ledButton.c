/*
 * 002_ledButton.c
 *
 *  Created on: Nov 25, 2025
 	Author: Rahul B.
 *  when button pressed led ON otherwise stop.
 *
 */

#include "bsp_init.h"

void delay(void){
	for(int i=0; i<500000/2; i++);
}

int main(){

	app_init();

	while(1){
		if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == BTN_PRESSED){
		    // Button is pressed: Turn LED ON
		    GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_SET);
		    delay();
		}else{
			// Button is NOT pressed: Turn LED OFF
		    GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_RESET);
		}
	}

	return 0;
}

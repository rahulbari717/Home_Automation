/*
 * 001_LedToggle.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari.
 */

#include "bsp_init.h"

void delay(void){
	for(int i=0; i<50000000; i++);
}

int main(){

	app_init();

	while(1){
		GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
		delay();
	}

	return 0;
}

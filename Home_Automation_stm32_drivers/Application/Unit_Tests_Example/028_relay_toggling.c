/*
 * 028_relay_toggling.c
 *
 *  Created on: Jan 25, 2026
 *      Author: lenovo
 */
#include "bsp_init.h"

USART_Handle_t usart2_handle;

void delay(void){
	for(int i=0; i<5000000; i++);
}

int main(){

	app_init();

	while(1){

		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_SET);
		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_13, GPIO_PIN_SET);
		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_14, GPIO_PIN_SET);
		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_15, GPIO_PIN_SET);
		delay();

		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_13, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_14, GPIO_PIN_RESET);
		GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_15, GPIO_PIN_RESET);
		delay();
	}

	return 0;
}

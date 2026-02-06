/*
 * 001_LedToggle.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari.
 */

#include "bsp_init.h"

int main(){

	app_init();


	while(1){
		// GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
		BSP_LED_Toggle(LED_GREEN_PIN);
		BSP_LED_Toggle(LED_RED_PIN);
		BSP_LED_Toggle(LED_WHITE_PIN);
		BSP_Delay_1s();
	}

	return 0;
}


/*
 * void BSP_LED_Init(void);
void BSP_LED_On(uint8_t PinNumber);
void BSP_LED_Off(uint8_t PinNumber);
void BSP_LED_Toggle(uint8_t pinNumber);

void BSP_LED_AllOff(void);
 *
 * */

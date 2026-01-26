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

/*
 * #include "bsp_button.h"
#include "bsp_led.h"

int main(void) {
    BSP_LED_Init();
    BSP_Button_Init();

    while(1) {
        if(BSP_Button_GetState() == BSP_BUTTON_PRESSED) {
            BSP_LED_Write(LED_GREEN_PIN, GPIO_PIN_SET);
        } else {
            BSP_LED_Write(LED_GREEN_PIN, GPIO_PIN_RESET);
        }
    }
}
 *
 *
 * */

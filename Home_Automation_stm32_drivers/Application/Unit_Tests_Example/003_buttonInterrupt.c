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

/*
 * #include "bsp_button.h"
#include "bsp_led.h"

int main(void) {
    BSP_LED_Init();
    BSP_Button_Init(); // This sets up the EXTI

    while(1);
    return 0;
}

// Override the callback to toggle the LED
void BSP_Button_Callback(void) {
    BSP_LED_Toggle(LED_GREEN_PIN);
}

// The actual ISR in main.c or it.c
void EXTI15_10_IRQHandler(void) {
    BSP_Button_IRQHandling();
}
 *
 * */

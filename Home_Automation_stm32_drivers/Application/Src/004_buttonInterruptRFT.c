/*
 * 004_buttonInterruptRFT.c
 * Fixed for: LED ON while holding button, OFF when released.
 */

#include "bsp_init.h"

void delay(void){
    // Simple delay for debounce
    for(int i=0; i<500000/2; i++);
}

int main(){
    app_init();
    while(1);
}

// IRQ Handler
void EXTI15_10_IRQHandler(void){

    // CHANGE 3: Check the actual pin state
    // We don't just toggle; we read the pin to see if it is currently High or Low.

    delay(); // Hardware debounce (simple method)

    GPIO_IRQHandling(GPIO_PIN_NO_13); // Clear the pending interrupt flag

    // PC13 is usually Active Low (0 = Pressed, 1 = Released)
    if(GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == 0) {
        // Button is Pressed (Low) -> Turn LED ON
        GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_SET);
    } else {
        // Button is Released (High) -> Turn LED OFF
        GPIO_WriteToOutputPin(GPIOA, GPIO_PIN_NO_5, GPIO_PIN_RESET);
    }
}

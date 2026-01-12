/*
 * 004_buttonInterruptRFT.c
 * Fixed for: LED ON while holding button, OFF when released.
 */

#include <stm32f446xx.h>
#include <string.h>

void delay(void){
    // Simple delay for debounce
    for(int i=0; i<500000/2; i++);
}

void init(void){
    GPIO_Handle_t GpioLed, GpioButton;
    memset(&GpioButton, 0, sizeof(GpioButton));
    memset(&GpioLed, 0, sizeof(GpioLed));

    // 1. Configure LED (PA5)
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // 2. Configure Button (PC13)
    GpioButton.pGPIOx = GPIOC;
    GpioButton.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;

    // CHANGE 1: Trigger interrupt on BOTH Rising and Falling edges
    // This allows us to detect when the button is pressed AND released.
    GpioButton.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IT_RFT;

    GpioButton.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioButton.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; // Ensure external pull-up exists, or change to PU

    GPIO_PeriClockControl(GPIOA, ENABLE);
    GPIO_PeriClockControl(GPIOC, ENABLE);

    GPIO_Init(&GpioLed);
    GPIO_Init(&GpioButton);

    // 3. Enable Interrupts
    GPIO_IRQPriorityConfig(EXTI15_10_IRQn, NVIC_IRQ_PRI15);
    GPIO_IRQInterruptConfig(EXTI15_10_IRQn, ENABLE);
}

int main(){
    init();

    // CHANGE 2: Infinite loop
    // The processor waits here endlessly. When button state changes,
    // it jumps to the ISR, does the work, and comes back here.
    while(1);

    return 0;
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

/*
 * 029_bsp_testing.c
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

/*
 * Unified BSP Test Suite
 */

#include "bsp_led.h"
#include "bsp_button.h"
#include "bsp_buzzer.h"
#include "bsp_delay.h"

// Shared state for non-blocking test
volatile uint8_t g_test_step = 0;

int main(void) {
    /* 1. INITIALIZATION */
    // Initialize Delay first as it sets the system clock to 180MHz
    BSP_Delay_Init_IT();
    BSP_Delay_Init();     // Initialize TIM2 blocking delays

    BSP_LED_Init();      // Setup PA5, PA6, PA7
    BSP_Buzzer_Init();   // Setup PA4
    BSP_Button_Init();   // Setup PC13 with Interrupts

    /* 2. BLOCKING DELAY & LED TEST */
    // Blink Green LED 3 times using blocking delay
    for(int i = 0; i < 3; i++) {
        BSP_LED_On(LED_GREEN_PIN);
        BSP_LED_On(LED_RED_PIN);
        BSP_LED_On(LED_WHITE_PIN);
        BSP_Delay_500ms();

        BSP_LED_Off(LED_GREEN_PIN);
        BSP_LED_Off(LED_RED_PIN);
        BSP_LED_Off(LED_WHITE_PIN);
        BSP_Delay_500ms();
    }

    /* 3. BUZZER TEST */
    // Test the Beep function (uses blocking delay internally)
    BSP_Buzzer_Beep(200);

    /* 4. NON-BLOCKING DELAY TEST (TIM7) */
    // Start a 3-second non-blocking delay
    BSP_DelaySec_IT(3);

    // While waiting for 3 seconds, toggle White LED fast
    while(!BSP_Delay_IsComplete()) {
        BSP_LED_Toggle(LED_WHITE_PIN);
        // Simple manual loop delay just for visibility during the wait
        for(int i = 0; i < 100000; i++);
    }
    BSP_LED_Off(LED_WHITE_PIN);

    /* 5. INTERRUPT BUTTON TEST */
    // Final state: Main loop is empty.
    // Pressing the button will trigger the ISR and toggle the RED LED.
    while(1) {
        // CPU is free to do background tasks here
    }

    return 0;
}

/**
 * @brief User Callback for Button Interrupt
 * Overrides the __weak function in bsp_button.c
 */
void BSP_Button_Callback(void) {
    BSP_LED_Toggle(LED_RED_PIN);
    // Beep briefly on every button press
    BSP_Buzzer_Toggle();
//    for(int i = 0; i < 100000; i++); // Very short delay
//     BSP_DelaySec_IT(3);
    BSP_Buzzer_Toggle();
}

/**
 * @brief EXTI15_10 ISR
 * Redirects the hardware interrupt to our BSP handler
 */
void EXTI15_10_IRQHandler(void) {
    BSP_Button_IRQHandling();
}

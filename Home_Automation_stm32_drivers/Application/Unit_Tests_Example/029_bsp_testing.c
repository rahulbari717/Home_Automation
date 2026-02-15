/*
 * 029_bsp_testing.c
 *
 * Created on: Feb 09, 2026
 * Author: Rahul Bari
 * Description: Simple ISR-based control system
 *
 * PC13 Button → Green LED + Buzzer
 * PC8 Button  → Red LED + Buzzer
 * PC6 IR      → White LED + Buzzer
 */


#include "bsp_led.h"
#include "bsp_button.h"
#include "bsp_buzzer.h"
#include "bsp_delay.h"
#include "bsp_ir_sensors.h"
#include "bsp_uart2_debug.h"

/* ===== MAIN ===== */
int main(void)
{
    /* 1. SYSTEM INITIALIZATION */
    BSP_Delay_Init();       // Initialize delay system (sets 180MHz clock)
    USART2_GPIOInit();
    USART2_Init();          // Initialize UART for debug
    BSP_LED_Init();         // Initialize LEDs (PA5, PA6, PA7)
    BSP_Buzzer_Init();      // Initialize Buzzer (PA4)
    BSP_Button_Init();      // Initialize PC13 button with EXTI
    PC8_Button_Init();      // Initialize PC8 button with EXTI
    PC6_IR_Init();          // Initialize PC6 IR sensor with EXTI

    UART_Printf("\r\n========================================\r\n");
    UART_Printf("  Multi-Input ISR Control System\r\n");
    UART_Printf("  Author: Rahul Bari\r\n");
    UART_Printf("========================================\r\n");
    UART_Printf("[INIT] System Ready!\r\n");
    UART_Printf("[INFO] PC13 Button -> Green LED + Buzzer\r\n");
    UART_Printf("[INFO] PC8 Button  -> Red LED + Buzzer\r\n");
    UART_Printf("[INFO] PC6 IR Sensor -> White LED + Buzzer\r\n\r\n");

    /* 2. STARTUP LED TEST */
    UART_Printf("[TEST] Running LED test...\r\n");
    for (int i = 0; i < 3; i++) {
        BSP_LED_On(LED_GREEN_PIN);
        BSP_LED_On(LED_RED_PIN);
        BSP_LED_On(LED_WHITE_PIN);
        BSP_Buzzer_On();
        BSP_Delay_1s();

        BSP_LED_Off(LED_GREEN_PIN);
        BSP_LED_Off(LED_RED_PIN);
        BSP_LED_Off(LED_WHITE_PIN);
        BSP_Buzzer_Off();
        BSP_Delay_1s();
    }
    
    UART_Printf("[TEST] LED test complete!\r\n\r\n");

    /* 3. MAIN LOOP - All control via ISRs */
    UART_Printf("[MAIN] Entering main loop. Waiting for interrupts...\r\n");
    while (1) {
        // CPU is free - all actions handled by interrupts
    }

    return 0;
}

/* ========================================================================
   INTERRUPT SERVICE ROUTINES
   ======================================================================== */

/**
 * @brief PC13 Button ISR (Green LED + Buzzer)
 * Handled by EXTI15_10
 */
void EXTI15_10_IRQHandler(void)
{
    // Debounce delay
    BSP_Delay_ms(50);

    // Clear pending bit
    GPIO_IRQHandling(GPIO_PIN_NO_13);

    // Check if button is still pressed (active LOW)
    if (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == GPIO_PIN_RESET) {
        // Button pressed - Green LED ON
        BSP_LED_On(LED_GREEN_PIN);
        BSP_Buzzer_On();
        UART_Printf("[ISR] PC13 Pressed → Green LED ON + Buzzer ON\r\n");

        BSP_Delay_ms(500);  // Keep buzzer on briefly
        BSP_Buzzer_Off();
    } else {
        // Button released - Green LED OFF
        BSP_LED_Off(LED_GREEN_PIN);
        UART_Printf("[ISR] PC13 Released → Green LED OFF\r\n");
    }
}

/**
 * @brief PC8 Button & PC6 IR ISR (Red/White LED + Buzzer)
 * Shared handler for EXTI9_5
 */
void EXTI9_5_IRQHandler(void)
{
    // Debounce delay
    BSP_Delay_ms(50);

    // Check which pin triggered the interrupt
    if (EXTI->PR & (1 << 8)) {  // PC8 (Button)
        // Clear pending bit
        GPIO_IRQHandling(GPIO_PIN_NO_8);

        // Check if button is pressed (active LOW)
        if (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_8) == GPIO_PIN_RESET) {
            // Button pressed - Red LED ON
            BSP_LED_On(LED_RED_PIN);
            BSP_Buzzer_On();
            UART_Printf("[ISR] PC8 Pressed → Red LED ON + Buzzer ON\r\n");

            BSP_Delay_ms(500);
            BSP_Buzzer_Off();

        } else {
            // Button released - Red LED OFF
            BSP_LED_Off(LED_RED_PIN);
            UART_Printf("[ISR] PC8 Released → Red LED OFF\r\n");
        }
    }

    if (EXTI->PR & (1 << 6)) {  // PC6 (IR Sensor)
        // Clear pending bit
        GPIO_IRQHandling(GPIO_PIN_NO_6);

        // Check if IR is detecting (active HIGH)
        if (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_6) == GPIO_PIN_RESET) {
            // Motion detected - White LED ON
            BSP_LED_On(LED_WHITE_PIN);
            BSP_Buzzer_On();
            UART_Printf("[ISR] PC6 Motion Detected → White LED ON + Buzzer ON\r\n");

            BSP_Delay_ms(500);
            BSP_Buzzer_Off();

        } else {
            // No motion - White LED OFF
            BSP_LED_Off(LED_WHITE_PIN);
            UART_Printf("[ISR] PC6 Motion Clear → White LED OFF\r\n");
        }
    }
}

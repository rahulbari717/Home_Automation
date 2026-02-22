/*
 * main.c
 * Project: Home Automation (Bare-Metal)
 * Author: Rahul Bari,
 * Date: Feb 12, 2026
 * * Hardware: STM32F446RE Nucleo-64
 * No HAL / No RTOS - Pure Register-Level Drivers
 */

#include "main.h"
#include "state_machine.h"
#include "bsp_uart2_debug.h"
#include "bsp_delay.h"
#include <stdbool.h>
#include <stdint.h>

/* ===== Global Variables ===== */

volatile uint8_t g_wakeup_flag = 0;
volatile bool pc13_event = false;
volatile bool pc8_intrusion_event = false;
volatile bool pc6_intrusion_event = false;
volatile uint32_t pc13_event_time = 0;

/* ================= MAIN APPLICATION ================= */
int main(void) {

    StateMachine_Init();

    // Peripheral Self-Test
    uint8_t test_result = System_SelfTest();
    if (test_result != 0) {
        UART_Printf(">> [WARNING] System test had errors (0x%X)\r\n", test_result);
        UART_Printf(">> [INFO] Continuing anyway...\r\n");
        Device_PlayBuzzer(BEEP_WARNING);
    } else {
        Device_PlayBuzzer(BEEP_SUCCESS);
        UART_Printf(">> [SUCCESS] All tests passed!\r\n");
    }

    UART_Printf(">> [SYSTEM] Entering main loop .\r\n");
    while(1) {
        StateMachine_Run();
    }
}

/* Interrupt Handler for Button */
void EXTI15_10_IRQHandler(void) {
    if(EXTI->PR & (1 << 13)) {
        EXTI->PR |= (1 << 13); // Clear pending bit
        g_wakeup_flag = 1;
    }
}

/**
 * @brief EXTI ISR for PC6 and PC8 (Intrusion Sensors)
 * EXTI lines 5–9 share this handler
 */
void EXTI9_5_IRQHandler(void)
{
    /* PC6 - IR Sensor 1 */
    if (EXTI->PR & (1 << 6))
    {
        UART_Printf("[ISR] PC6 IR Sensor triggered!\r\n");
        EXTI->PR |= (1 << 6);     // Clear pending bit
        pc6_intrusion_event = true;
    }

    /* PC8 - IR Sensor 2 */
    if (EXTI->PR & (1 << 8))
    {
        UART_Printf("[ISR] PC8 triggered!\r\n");
        EXTI->PR |= (1 << 8);     // Clear pending bit
        pc8_intrusion_event = true;
    }
}

/*
 * 008_fault_test.c
 *
 * Created on: Jan 16, 2026
 * Author: Rahul B.
 * Description: Interactive Fault Injection Tester for The Citadel
 *
 * Requirements:
 * 1. UART Driver (USART2) must be implemented.
 * 2. Fault Handler Driver must be implemented and included.
 * 3. Fault_Enable_Traps() must be available in the driver to set SCB->CCR bits.
 */

#include "stm32f446xx.h"
#include "bsp_uart2_debug.h"

void test_fault();

int main(void)
{
    // 1. Init System
    Debug_Init();        // Initialize UART2 via BSP
    test_fault();

}

void test_fault(){
    // 2. UI Setup
    Debug_ClearScreen();
    UART_Printf("========================================\r\n");
    UART_Printf(">> [TEST] The Citadel: Fault Injection Tool\r\n");
    UART_Printf(">> [INFO] Traps Enabled: DivByZero, Unaligned\r\n");
    UART_Printf("========================================\r\n");

    while(1)
    {
        UART_Printf("\r\n--- Select a Fault to Trigger ---\r\n");
        UART_Printf("  [1] UsageFault: Divide by Zero\r\n");
        UART_Printf("  [2] UsageFault: Unaligned Memory Access\r\n");
        UART_Printf("  [3] UsageFault: Invalid Instruction\r\n");
        UART_Printf("  [4] BusFault:   Invalid Memory Read\r\n");
        UART_Printf("  [5] Safe Mode:  Print 'System Healthy'\r\n");
        UART_Printf(">> Enter Choice: ");

        uint8_t choice = UART_ReceiveByte();
        UART_Printf("%c\r\n", choice); // Echo user input

        switch(choice)
        {
            case '1':
                UART_Printf(">> [CMD] Triggering Divide By Zero...\r\n");
                // Small delay to ensure UART TX finishes before crash
                for(volatile int i=0; i<10000; i++);
                Trigger_DivideByZero();
                break;

            case '2':
                UART_Printf(">> [CMD] Triggering Unaligned Access...\r\n");
                for(volatile int i=0; i<10000; i++);
                Trigger_UnalignedAccess();
                break;

            case '3':
                UART_Printf(">> [CMD] Triggering Invalid Instruction...\r\n");
                for(volatile int i=0; i<10000; i++);
                Trigger_InvalidInstruction();
                break;

            case '4':
                UART_Printf(">> [CMD] Triggering Bus Fault...\r\n");
                for(volatile int i=0; i<10000; i++);
                Trigger_BusFault();
                break;

            case '5':
                UART_Printf(">> [INFO] System is Healthy.\r\n");
                break;

            default:
                UART_Printf(">> [ERR] Invalid Option.\r\n");
                break;
        }
    }
}

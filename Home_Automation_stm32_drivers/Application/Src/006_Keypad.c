/*
 * keypad_uart_test.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari
 */

#include "bsp_init.h"
#include "bsp_keypad.h"
USART_Handle_t usart2_handle;
/* ===== MAIN APPLICATION ===== */
int main(void)
{
    char key;
    app_init();

    // Print welcome message
    UART_Printf("\r\n\r\n========================================\r\n");
    UART_Printf(">> [BOOT] System ON\r\n");
    UART_Printf(">> 4x4 Keypad Test Application\r\n");
    UART_Printf("========================================\r\n\r\n");
    UART_Printf("Keypad Layout:\r\n");
    UART_Printf("  C0  C1  C2  C3\r\n");
    UART_Printf("R0 1   2   3   A\r\n");
    UART_Printf("R1 4   5   6   B\r\n");
    UART_Printf("R2 7   8   9   C\r\n");
    UART_Printf("R3 *   0   #   D\r\n\r\n");
    UART_Printf("Press any key on the keypad...\r\n\r\n");
    
    // 4. Main loop - Scan keypad and print to UART
    while(1)
    {
        key = Keypad_GetKey();
        
        if(key != KEYPAD_NO_KEY)
        {
            UART_Printf("Key Pressed: %c\r\n", key);
        }
        
        // Small delay to avoid flooding UART
        for(volatile uint32_t i = 0; i < 100000; i++);
    }
}

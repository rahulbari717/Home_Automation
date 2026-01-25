/*
 * 025_bluetooth_test.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Bluetooth HC-05 Test Application
 */

#include "main.h"
#include "bsp_bluetooth.h"
#include "bsp_uart2_debug.h"

int main(void)
{
    // Initialize
    Debug_Init();
    BSP_Bluetooth_Init();
    
    UART_Printf("\r\n===== Bluetooth HC-05 Test =====\r\n");
    UART_Printf("Waiting for Bluetooth connection...\r\n");
    
    // Send test message
    BSP_Bluetooth_Printf("Hello from STM32!\r\n");
    BSP_Bluetooth_Printf("System Ready\r\n");
    
    while(1)
    {
        // Echo received data back
        if(BSP_Bluetooth_DataAvailable())
        {
            uint8_t received = BSP_Bluetooth_ReceiveByte();
            
            // Log to debug UART
            UART_Printf("BT RX: %c\r\n", received);
            
            // Echo back via Bluetooth
            BSP_Bluetooth_Printf("You sent: %c\r\n", received);
        }
    }
}
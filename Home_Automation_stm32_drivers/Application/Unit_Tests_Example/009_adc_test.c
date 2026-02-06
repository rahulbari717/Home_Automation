/*
 * 009_adc_test.c
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 * Description: ADC Sensor Test (Pot & LDR) with UART Output
 */

#include "bsp_init.h"
#include "bsp_uart2_debug.h"
#include "bsp_adc_sensors.h"

USART_Handle_t usart2_handle;

int main(void)
{
    app_init(); // This calls LDR_init() inside bsp_init.c

    Debug_ClearScreen();
    UART_Printf("========================================\r\n");
    UART_Printf(">> [TEST] Dual LDR Monitor Started      \r\n");
    UART_Printf(">> LDR1: PA0 (CH0) | LDR2: PA1 (CH1)    \r\n");
    UART_Printf("========================================\r\n");

    uint16_t ldr1 = 0, ldr2 = 0;

    while(1)
    {
        // Read LDR 1 using the channel macro from config.h
        ldr1 = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);

        // Read LDR 2
        ldr2 = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);

        // Print values formatted nicely
        UART_Printf("LDR1 (Hall): %4d | LDR2 (Gate): %4d\r\n", ldr1, ldr2);

        // Delay 500ms (approx)
        for(volatile uint32_t i = 0; i < 500000; i++);
    }
}

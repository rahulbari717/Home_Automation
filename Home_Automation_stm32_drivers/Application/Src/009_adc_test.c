/*
 * 009_adc_test.c
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 * Description: ADC Sensor Test (Pot & LDR) with UART Output
 */

#include "stm32f446xx.h"
#include "bsp_uart2_debug.h"
#include "bsp_adc_sensors.h"

// Small helper to implement a blocking delay
void delay_ms(uint32_t count) {
    for(uint32_t i = 0; i < (count * 1000); i++);
}

int main(void)
{
    // 1. System Initialization
    Debug_Init();         // Initialize UART2
    BSP_Sensors_Init();   // Initialize ADC & GPIOs for Pot/LDR

    // 2. UI Setup
    Debug_ClearScreen();
    UART_Printf("========================================\r\n");
    UART_Printf(">> [TEST] The Citadel: Sensor Monitor   \r\n");
    UART_Printf(">> [INFO] LDR: PA1 (ADC1_IN1)           \r\n");
    UART_Printf("========================================\r\n");


    uint16_t ldr_val = 0;

        while(1)
        {
            // 3. Read LDR Value
            ldr_val = BSP_Sensor_ReadLDR();

            // 4. Print value
            // Dark  = High Resistance = Voltage changes (usually lower value)
            // Light = Low Resistance  = Voltage changes (usually higher value)
            UART_Printf("LDR Value: %4d\r\n", ldr_val);

            // 5. Delay 500ms
            delay_ms(500);
        }
}

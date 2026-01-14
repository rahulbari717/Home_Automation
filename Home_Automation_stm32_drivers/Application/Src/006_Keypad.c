/*
 * keypad_uart_test.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari
 */

#include "stm32f446xx.h"
#include "keypad.h"

USART_Handle_t usart2_handle;

/* ===== USART2 GPIO INITIALIZATION ===== */
void USART2_GPIOInit(void)
{
    GPIO_Handle_t usart_gpios;
    usart_gpios.pGPIOx = GPIOA;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

    // TX - PA2
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2;
    GPIO_Init(&usart_gpios);
    
    // RX - PA3
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
    GPIO_Init(&usart_gpios);
}

/* ===== USART2 INITIALIZATION ===== */
void USART2_Init(void)
{
    usart2_handle.pUSARTx = USART2;
    usart2_handle.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    usart2_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    usart2_handle.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
    usart2_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    usart2_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    usart2_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    USART_Init(&usart2_handle);
}

/* ===== DEBUG PRINTF FUNCTION ===== */
void UART_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    USART_SendData(&usart2_handle, (uint8_t*)buffer, strlen(buffer));
    va_end(args);
}

/* ===== MAIN APPLICATION ===== */
int main(void)
{
    char key;
    
    // 1. Initialize UART for debugging
    USART2_GPIOInit();
    USART2_Init();
    USART_PeripheralControl(USART2, ENABLE);
    
    // 2. Initialize Keypad
    Keypad_Init();
    
    // 3. Print welcome message
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

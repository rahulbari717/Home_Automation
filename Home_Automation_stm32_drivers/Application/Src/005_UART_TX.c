/*
 * 005_UART_TX.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari.
 */

#include "stm32f446xx.h"

char msg[1024] = "UART TX Testing ... \n\r";
USART_Handle_t usart2_handle;

void USART2_GPIOInit(void)
{
    GPIO_Handle_t usart_gpios;
    usart_gpios.pGPIOx = GPIOA;
    usart_gpios.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    usart_gpios.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    usart_gpios.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    usart_gpios.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    usart_gpios.GPIO_PinConfig.GPIO_PinAltFunMode = 7;

    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_2; // TX
    GPIO_Init(&usart_gpios);
    usart_gpios.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3; // RX
    GPIO_Init(&usart_gpios);
}

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

/* ===== DEBUG FUNCTION ===== */
void UART_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    USART_SendData(&usart2_handle, (uint8_t*)buffer, strlen(buffer));
    va_end(args);
}

/* ===== MAIN ===== */
int main(void)
{
    // 1. Init UART First for Debugging
    USART2_GPIOInit();
    USART2_Init();
    USART_PeripheralControl(USART2, ENABLE);

    UART_Printf("\r\n\r\n========================================\r\n");
    UART_Printf(">> [BOOT] System ON .\r\n");

    UART_Printf("%s \n ", msg);


}

/*
 * bsp_bluetooth.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 */

#include "bsp_bluetooth.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"
#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Private Handle
static USART_Handle_t bt_usart_handle;

// Private Helper: Initialize GPIO pins (PA9=TX, PA10=RX)
static void Bluetooth_GPIO_Init(void)
{
    GPIO_Handle_t bt_gpio;
    
    bt_gpio.pGPIOx = BT_USART_PORT;
    bt_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    bt_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    bt_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    bt_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    bt_gpio.GPIO_PinConfig.GPIO_PinAltFunMode = BT_USART_AF;
    
    // TX Pin (PA9)
    bt_gpio.GPIO_PinConfig.GPIO_PinNumber = BT_TX_PIN;
    GPIO_Init(&bt_gpio);
    
    // RX Pin (PA10)
    bt_gpio.GPIO_PinConfig.GPIO_PinNumber = BT_RX_PIN;
    GPIO_Init(&bt_gpio);
}

void BSP_Bluetooth_Init(void)
{
    // 1. Init GPIO
    Bluetooth_GPIO_Init();
    
    // 2. Configure USART1
    bt_usart_handle.pUSARTx = BT_USART;
    bt_usart_handle.USART_Config.USART_Baud = BT_BAUD_RATE;
    bt_usart_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    bt_usart_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    bt_usart_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    bt_usart_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    bt_usart_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    
    USART_Init(&bt_usart_handle);
    USART_PeripheralControl(BT_USART, ENABLE);
}

void BSP_Bluetooth_SendString(const char *str)
{
    USART_SendData(&bt_usart_handle, (uint8_t*)str, strlen(str));
}

void BSP_Bluetooth_Printf(const char *format, ...)
{
    char buffer[128];
    va_list args;
    
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    BSP_Bluetooth_SendString(buffer);
}

uint8_t BSP_Bluetooth_ReceiveByte(void)
{
    uint8_t data;
    USART_ReceiveData(&bt_usart_handle, &data, 1);
    return data;
}

uint8_t BSP_Bluetooth_DataAvailable(void)
{
    // Check RXNE (Read Data Register Not Empty) flag
    return (BT_USART->SR & (1 << 5)) ? 1 : 0;
}

void BSP_Bluetooth_SendATCommand(const char *cmd)
{
    BSP_Bluetooth_SendString(cmd);
    BSP_Bluetooth_SendString("\r\n");
}
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

extern USART_Handle_t bt_handle;

static void Bluetooth_GPIO_Init(void)
{
    GPIO_Handle_t bt_gpio;
    bt_gpio.pGPIOx = BT_USART_PORT;
    bt_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
    bt_gpio.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    bt_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
    bt_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    bt_gpio.GPIO_PinConfig.GPIO_PinAltFunMode = BT_USART_AF;
    
    bt_gpio.GPIO_PinConfig.GPIO_PinNumber = BT_TX_PIN;
    GPIO_Init(&bt_gpio);
    
    bt_gpio.GPIO_PinConfig.GPIO_PinNumber = BT_RX_PIN;
    GPIO_Init(&bt_gpio);
}

void BSP_Bluetooth_Init(void)
{
    Bluetooth_GPIO_Init();
    
    memset(&bt_handle, 0, sizeof(bt_handle));
    bt_handle.pUSARTx = BT_USART;
    bt_handle.USART_Config.USART_Baud = BT_BAUD_RATE;
    bt_handle.USART_Config.USART_Mode = USART_MODE_TXRX;
    bt_handle.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    bt_handle.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    bt_handle.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    bt_handle.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    
    USART_Init(&bt_handle);
    USART_PeripheralControl(BT_USART, ENABLE);
}

void BSP_Bluetooth_SendString(const char *str)
{
    USART_SendData(&bt_handle, (uint8_t*)str, strlen(str));
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
    USART_ReceiveData(&bt_handle, &data, 1);
    return data;
}

uint8_t BSP_Bluetooth_DataAvailable(void)
{
    return (BT_USART->SR & (1 << 5)) ? 1 : 0;
}

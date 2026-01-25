/*
 * 005_UART_TX.c
 *
 *  Created on: Jan 13, 2026
 *      Author: Rahul Bari.
 */

#include "bsp_init.h"

char msg[1024] = "UART TX Testing ... \n\r";
USART_Handle_t usart2_handle;

/* ===== MAIN ===== */
int main(void)
{
    // 1. Init UART First for Debugging
    app_init();

    UART_Printf(">> [BOOT] System ON .\r\n");

    UART_Printf("%s \n ", msg);

}

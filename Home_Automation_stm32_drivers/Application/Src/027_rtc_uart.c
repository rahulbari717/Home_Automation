/*
 * 007_rtc_uart.c
 *
 *  Created on: Jan 14, 2026
 *      Author: Rahul Bari
 */

#include "bsp_init.h"
#include "bsp_uart2_debug.h"

char msg[1024] = "UART TX Testing ... \n\r";

USART_Handle_t usart2_handle;
RTC_Handle_t rtc_handle;

// --- Input Helper ---
uint8_t Get_Number_Input(uint8_t max_digits)
{
    char input[10] = {0};
    uint8_t idx = 0;
    uint8_t ch;
    while(1)
    {
        ch = UART_ReceiveByte();
        if(ch == '\r' || ch == '\n') {
            UART_Printf("\r\n");
            break;
        }
        else if(ch == 8 || ch == 127) { // Backspace
            if(idx > 0) {
                idx--;
                UART_Printf("\b \b");
            }
        }
        else if(ch >= '0' && ch <= '9' && idx < max_digits) {
            input[idx++] = ch;
            USART_SendData(&usart2_handle, &ch, 1);
        }
    }
    input[idx] = '\0';
    return (uint8_t)atoi(input);
}

void Delay_Ms(uint32_t ms)
{
    for(uint32_t i = 0; i < ms; i++)
    {
        for(volatile uint32_t j = 0; j < 1600; j++);
    }
}

/* ===== MAIN ===== */
int main(void)
{
    uint8_t hours, minutes, seconds;
    uint8_t year, month, date, weekday;

    const char *days[] = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    Fault_Init();

    // Initialize UART
    USART2_GPIOInit();
    USART2_Init();
	USART_PeripheralControl(USART2, ENABLE);

	UART_Printf("\r\n========================================\r\n");
	UART_Printf(">> [BOOT] System ON\r\n");

	// Initialize RTC using DRIVER (Not local function)
	rtc_handle.pRTC = RTC;
	RTC_Init(&rtc_handle);

	UART_Printf(">> [APP] RTC Init Completed.\r\n");
	UART_Printf("========================================\r\n");

	// Get Date Input
	UART_Printf("\r\nEnter Current Date:\r\n");
	UART_Printf("  Day (1-31): ");
	date = Get_Number_Input(2);
	UART_Printf("  Month (1-12): ");
	month = Get_Number_Input(2);
	UART_Printf("  Year (00-99): ");
	year = Get_Number_Input(2);
	UART_Printf("  Weekday (1=Mon, 7=Sun): ");
	weekday = Get_Number_Input(1);

	// Get Time Input
	UART_Printf("\r\nEnter Current Time:\r\n");
	UART_Printf("  Hours (0-23): ");
	hours = Get_Number_Input(2);
	UART_Printf("  Minutes (0-59): ");
	minutes = Get_Number_Input(2);
	UART_Printf("  Seconds (0-59): ");
	seconds = Get_Number_Input(2);

	// Set RTC
	RTC_SetDate(&rtc_handle, year, month, date, weekday);
	RTC_SetTime(&rtc_handle, hours, minutes, seconds);

	UART_Printf("\r\n>> RTC Configured Successfully!\r\n");

	for(int i =0; i<5; i++)
	{
		RTC_GetTime(&rtc_handle, &hours, &minutes, &seconds);
		RTC_GetDate(&rtc_handle, &year, &month, &date, &weekday);

		UART_Printf("Date: %02d/%02d/20%02d (%s)  Time: %02d:%02d:%02d\r",
					date, month, year, days[weekday], hours, minutes, seconds);

		Delay_Ms(1000);
	}
	UART_Printf("\r\n>> application exit...!\r\n");
	while(1);
}

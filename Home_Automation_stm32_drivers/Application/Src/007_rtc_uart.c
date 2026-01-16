/*
 * 007_rtc_uart.c
 *
 *  Created on: Jan 14, 2026
 *      Author: Rahul Bari
 */

#include "stm32f446xx.h"

char msg[1024] = "UART TX Testing ... \n\r";

USART_Handle_t usart2_handle;
RTC_Handle_t rtc_handle;

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
    usart2_handle.USART_Config.USART_Mode = USART_MODE_TXRX; // Changed to TX+RX
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

/* ===== UART RECEIVE FUNCTION ===== */
uint8_t UART_ReceiveByte(void)
{
    uint8_t data;
    USART_ReceiveData(&usart2_handle, &data, 1);
    return data;
}

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

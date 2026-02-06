/*
 * 21_oled_clock.c
 *
 * Description: Real Time Clock display on OLED.
 * Integrates: RTC Driver, UART Driver, and OLED BSP.
 *
 * Created on: Jan 23, 2026
 * Author: Rahul B.
 */

#include "bsp_init.h"
#include "bsp_uart2_debug.h"
#include "bsp_i2c_oled.h"

// --- Global Handles ---
RTC_Handle_t rtc_handle;
const char *days[] = {"", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
USART_Handle_t usart2_handle;

// --- Helper Functions (Copied from your code) ---
uint8_t Get_Number_Input(uint8_t max_digits) {
    char input[10] = {0};
    uint8_t idx = 0;
    uint8_t ch;
    while(1) {
        // Assuming Debug_ReceiveByte is wrapper for USART_ReceiveData
        // If not available, use your direct USART calls
        ch = UART_ReceiveByte();
        if(ch == '\r' || ch == '\n') { UART_Printf("\r\n"); break; }
        else if(ch == 8 || ch == 127) {
            if(idx > 0) { idx--; UART_Printf("\b \b"); }
        }
        else if(ch >= '0' && ch <= '9' && idx < max_digits) {
            input[idx++] = ch;
            USART_SendData(Debug_GetHandle(), (uint8_t*)&ch, 1);
        }
    }
    input[idx] = '\0';
    return (uint8_t)atoi(input);
}

void RTC_Setup_Interactive(void) {
    uint8_t h, m, s, D, M, Y, W;

    rtc_handle.pRTC = RTC;
    RTC_Init(&rtc_handle);

    UART_Printf(">> [SETUP] Enter Current Time:\r\n");
    UART_Printf("   Hour (0-23): "); h = Get_Number_Input(2);
    UART_Printf("   Min  (0-59): "); m = Get_Number_Input(2);
    UART_Printf("   Sec  (0-59): "); s = Get_Number_Input(2);

    UART_Printf("   Day  (1-31): "); D = Get_Number_Input(2);
    UART_Printf("   Mon  (1-12): "); M = Get_Number_Input(2);
    UART_Printf("   Year (00-99): "); Y = Get_Number_Input(2);
    UART_Printf("   Week (1=Mon): "); W = Get_Number_Input(1);

    RTC_SetTime(&rtc_handle, h, m, s);
    RTC_SetDate(&rtc_handle, Y, M, D, W);

    UART_Printf(">> [INFO] RTC Configured.\r\n");
}

int main(void)
{
    app_init();
    // 1. Initialize Drivers
    TIMER_DelayInit();
    BSP_OLED_Init();      // Init OLED

    // 2. Clear Screen
    Debug_ClearScreen();
    UART_Printf("============================\r\n");
    UART_Printf(">> OLED Digital Clock       \r\n");
    UART_Printf("============================\r\n");

    // 3. Setup Time
    RTC_Setup_Interactive();

    // 4. Main Loop
    uint8_t h, m, s, D, M, Y, W;

    while(1)
    {
        // A. Read Time
        RTC_GetTime(&rtc_handle, &h, &m, &s);
        RTC_GetDate(&rtc_handle, &Y, &M, &D, &W);

        // B. Update OLED Buffer
        BSP_OLED_Clear();

        // Print Header (Inverted colors logic if you had it, or just text)
        OLED_Printf(0, 0, ">> Rahul's CLOCK <<");

        // Print Time (Large format if font supported, otherwise standard)
        // Centering text: (128 - (8chars * 5px)) / 2 = approx x=40
        OLED_Printf(30, 20, "%02d:%02d:%02d", h, m, s);

        // Print Date
        OLED_Printf(10, 40, "%s, %02d/%02d/20%02d", days[W], D, M, Y);

        // C. Push to Display
        BSP_OLED_Update();

        // D. Wait 1 second
        // Note: Using 990ms to account for processing time so seconds tick smoothly
        TIMER_DelayMs(TIM2, 990);

        // Optional: Toggle LED to show heartbeat
        // GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
    }
}

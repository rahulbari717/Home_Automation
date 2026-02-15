/*
 * devices.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Device Control Functions (LEDs, Relays, Buzzer)
 */

#include "state_machine.h"
#include "bsp_led.h"
#include "bsp_relay.h"
#include "bsp_buzzer.h"
#include "bsp_uart2_debug.h"
#include "bsp_delay.h"
#include <stdio.h>

/* ===== BUZZER CONTROL ===== */

/**
 * @brief Play buzzer pattern
 */
void Device_PlayBuzzer(BuzzerPattern_t pattern)
{
    switch (pattern) {
        case BEEP_SUCCESS:
            // Single short beep (200ms)
            BSP_Buzzer_On();
            BSP_Delay_ms(300);
            BSP_Buzzer_Off();
            UART_Printf("[DEVICE] Buzzer: SUCCESS\r\n");
            break;

        case BEEP_ERROR:
            // Two short beeps (100ms each)
            for (int i = 0; i < 2; i++) {
                BSP_Buzzer_On();
                BSP_Delay_ms(200);
                BSP_Buzzer_Off();
                BSP_Delay_ms(200);
            }
            UART_Printf("[DEVICE] Buzzer: ERROR\r\n");
            break;

        case BEEP_WARNING:
            // Three short beeps
            for (int i = 0; i < 3; i++) {
                BSP_Buzzer_On();
                BSP_Delay_ms(200);
                BSP_Buzzer_Off();
                BSP_Delay_ms(200);
            }
            UART_Printf("[DEVICE] Buzzer: WARNING\r\n");
            break;

        case BEEP_ALARM:
            // Three long beeps for alarm
            for (int i = 0; i < 3; i++) {
                BSP_Buzzer_On();
                BSP_Delay_ms(500);
                BSP_Buzzer_Off();
                BSP_Delay_ms(200);
            }
            UART_Printf("[DEVICE] Buzzer: ALARM\r\n");
            break;

        case BEEP_MOTION:
            // Single long beep (500ms)
            BSP_Buzzer_On();
            BSP_Delay_ms(500);
            BSP_Buzzer_Off();
            UART_Printf("[DEVICE] Buzzer: MOTION\r\n");
            break;

        default:
            break;
    }
}

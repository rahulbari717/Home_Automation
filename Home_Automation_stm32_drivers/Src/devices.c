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

/* ===== LED CONTROL ===== */

/**
 * @brief Toggle LED and update device state
 */
void Device_ToggleLED(uint8_t pin)
{
    BSP_LED_Toggle(pin);
    
    // Update device state
    bool newState = !GPIO_ReadFromInputPin(LED_PORT, pin);
    
    if (pin == LED_GREEN_PIN) {
        g_DeviceStates.led_green = newState;
        UART_Printf("[DEVICE] LED Green: %s\r\n", newState ? "ON" : "OFF");
    } else if (pin == LED_RED_PIN) {
        g_DeviceStates.led_red = newState;
        UART_Printf("[DEVICE] LED Red: %s\r\n", newState ? "ON" : "OFF");
    } else if (pin == LED_WHITE_PIN) {
        g_DeviceStates.led_white = newState;
        UART_Printf("[DEVICE] LED White: %s\r\n", newState ? "ON" : "OFF");
    }
}

/* ===== RELAY CONTROL ===== */

/**
 * @brief Toggle Relay and update device state
 */
void Device_ToggleRelay(uint8_t pin)
{
    BSP_Relay_Toggle(pin);
    
    // Update device state
    bool newState = GPIO_ReadFromInputPin(RELAY_PORT, pin);
    
    if (pin == RELAY1_PIN) {
        g_DeviceStates.relay1 = newState;
        UART_Printf("[DEVICE] Relay1: %s\r\n", newState ? "ON" : "OFF");
    } else if (pin == RELAY2_PIN) {
        g_DeviceStates.relay2 = newState;
        UART_Printf("[DEVICE] Relay2: %s\r\n", newState ? "ON" : "OFF");
    } else if (pin == RELAY3_PIN) {
        g_DeviceStates.relay3 = newState;
        UART_Printf("[DEVICE] Relay3: %s\r\n", newState ? "ON" : "OFF");
    } else if (pin == RELAY4_PIN) {
        g_DeviceStates.relay4 = newState;
        UART_Printf("[DEVICE] Relay4: %s\r\n", newState ? "ON" : "OFF");
    }
}

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
            BSP_Delay_ms(200);
            BSP_Buzzer_Off();
            UART_Printf("[DEVICE] Buzzer: SUCCESS\r\n");
            break;
            
        case BEEP_ERROR:
            // Two short beeps (100ms each)
            for (int i = 0; i < 2; i++) {
                BSP_Buzzer_On();
                BSP_Delay_ms(100);
                BSP_Buzzer_Off();
                BSP_Delay_ms(100);
            }
            UART_Printf("[DEVICE] Buzzer: ERROR\r\n");
            break;
            
        case BEEP_WARNING:
            // Three short beeps
            for (int i = 0; i < 3; i++) {
                BSP_Buzzer_On();
                BSP_Delay_ms(100);
                BSP_Buzzer_Off();
                BSP_Delay_ms(100);
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

/* ===== LDR AUTO MODE ===== */

/**
 * @brief Automatic device control based on LDR values
 */
void Device_UpdateLDRAutoMode(void)
{
    static bool ldr1_dark_state = false;
    static bool ldr2_dark_state = false;
    
    // LDR1 Control - White LED and Relay1
    bool ldr1_is_dark = (g_SensorData.ldr1_value < LDR_DARK_THRESHOLD);
    
    if (ldr1_is_dark && !ldr1_dark_state) {
        // Transition to dark
        BSP_LED_On(LED_WHITE_PIN);
        BSP_Relay_SetState(RELAY1_PIN, GPIO_PIN_SET);
        g_DeviceStates.led_white = true;
        g_DeviceStates.relay1 = true;
        UART_Printf("[AUTO] LDR1 Dark (%u) -> White LED & Relay1 ON\r\n", 
                    g_SensorData.ldr1_value);
        ldr1_dark_state = true;
    } else if (!ldr1_is_dark && ldr1_dark_state) {
        // Transition to bright
        BSP_LED_Off(LED_WHITE_PIN);
        BSP_Relay_SetState(RELAY1_PIN, GPIO_PIN_RESET);
        g_DeviceStates.led_white = false;
        g_DeviceStates.relay1 = false;
        UART_Printf("[AUTO] LDR1 Bright (%u) -> White LED & Relay1 OFF\r\n", 
                    g_SensorData.ldr1_value);
        ldr1_dark_state = false;
    }
    
    // LDR2 Control - Red LED (Warning)
    bool ldr2_is_dark = (g_SensorData.ldr2_value < LDR_DARK_THRESHOLD);
    
    if (ldr2_is_dark && !ldr2_dark_state) {
        // Transition to dark
        BSP_LED_On(LED_RED_PIN);
        g_DeviceStates.led_red = true;
        UART_Printf("[AUTO] LDR2 Dark (%u) -> Red LED ON (Warning)\r\n", 
                    g_SensorData.ldr2_value);
        ldr2_dark_state = true;
    } else if (!ldr2_is_dark && ldr2_dark_state) {
        // Transition to bright
        BSP_LED_Off(LED_RED_PIN);
        g_DeviceStates.led_red = false;
        UART_Printf("[AUTO] LDR2 Bright (%u) -> Red LED OFF\r\n", 
                    g_SensorData.ldr2_value);
        ldr2_dark_state = false;
    }
}

/* ===== DEVICE STATUS ===== */

/**
 * @brief Send device status via UART
 */
void Device_SendStatusUART(void)
{
    UART_Printf("[DEVICE] === Device Status ===\r\n");
    UART_Printf("[DEVICE] LEDs - Green:%s Red:%s White:%s\r\n",
                g_DeviceStates.led_green ? "ON" : "OFF",
                g_DeviceStates.led_red ? "ON" : "OFF",
                g_DeviceStates.led_white ? "ON" : "OFF");
    
    UART_Printf("[DEVICE] Relays - R1:%s R2:%s R3:%s R4:%s\r\n",
                g_DeviceStates.relay1 ? "ON" : "OFF",
                g_DeviceStates.relay2 ? "ON" : "OFF",
                g_DeviceStates.relay3 ? "ON" : "OFF",
                g_DeviceStates.relay4 ? "ON" : "OFF");
    
    UART_Printf("[DEVICE] LDR Auto Mode: %s\r\n",
                g_DeviceStates.ldr_auto_mode ? "ENABLED" : "DISABLED");
    
    UART_Printf("[DEVICE] =======================\r\n");
}
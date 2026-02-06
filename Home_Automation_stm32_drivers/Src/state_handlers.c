/*
 * state_handlers.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Individual State Handler Functions
 */

#include "state_machine.h"
#include "bsp_ir_sensors.h"
#include "bsp_keypad.h"
#include "bsp_led.h"
#include "bsp_lcd.h"
#include "bsp_i2c_oled.h"
#include "bsp_uart2_debug.h"
#include "bsp_delay.h"

#include <stdio.h>
#include <string.h>

/* ===== STATE: STANDBY ===== */

/**
 * @brief Standby mode - Wait for user interaction
 */
void Handle_Standby(void)
{
    // Check for keypad input
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        UART_Printf("[STANDBY] Key pressed: %c. Transitioning to AUTH.\r\n", key);
        StateMachine_SetState(STATE_AUTHENTICATING);
        return;
    }
    
    // Check for IR sensor triggers
    if (BSP_Sensor_Read_IR1() || BSP_Sensor_Read_IR2()) {
        UART_Printf("[STANDBY] IR sensor triggered.\r\n");
        StateMachine_SetState(STATE_AUTHENTICATING);
        return;
    }
    
    // Periodic status update
    static uint32_t lastStatusUpdate = 0;
    if (CheckTimeout(lastStatusUpdate, 10000)) { // Every 10 seconds
        UART_Printf("[STANDBY] System ready. IR1:%d IR2:%d \r\n",
                    g_SensorData.ir1_detected,
                    g_SensorData.ir2_detected);
        lastStatusUpdate = GetSystemTick();
    }
}

/* ===== STATE: AUTHENTICATING ===== */

/**
 * @brief Authentication mode - PIN entry and verification
 */
void Handle_Authentication(void)
{
    // Check for timeout
    if (CheckTimeout(g_SystemContext.lastActivityTime, SCREEN_TIMEOUT_SEC * 1000)) {
        UART_Printf("[AUTH] Timeout. Returning to STANDBY.\r\n");
        StateMachine_SetState(STATE_STANDBY);
        return;
    }
    
    // Get keypad input
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        Auth_ProcessKeyPress(key);
        g_SystemContext.lastActivityTime = GetSystemTick();
    }
}

/* ===== STATE: ACTIVE MENU ===== */

/**
 * @brief Active menu - Main menu navigation
 */
void Handle_ActiveMenu(void)
{
    // Check for timeout
    if (CheckTimeout(g_SystemContext.lastActivityTime, SCREEN_TIMEOUT_SEC * 1000)) {
        UART_Printf("[MENU] Timeout. Logging out.\r\n");
        StateMachine_SetState(STATE_STANDBY);
        return;
    }
    
    // Get keypad input
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        g_SystemContext.lastActivityTime = GetSystemTick();
        
        switch (key) {
            case KEY_UP:        // '2'
                if (g_SystemContext.menuCursor > 0) {
                    g_SystemContext.menuCursor--;
                    UART_Printf("[MENU] Cursor UP: %d\r\n", g_SystemContext.menuCursor);
                }
                break;
                
            case KEY_DOWN:      // '8'
                if (g_SystemContext.menuCursor < 3) {
                    g_SystemContext.menuCursor++;
                    UART_Printf("[MENU] Cursor DOWN: %d\r\n", g_SystemContext.menuCursor);
                }
                break;
                
            case KEY_ENTER:     // '5'
                UART_Printf("[MENU] Selection: %d\r\n", g_SystemContext.menuCursor);
                Menu_ProcessSelection();
                break;
                
            case KEY_LOGOUT:    // '#'
                UART_Printf("[MENU] Logout requested.\r\n");
                StateMachine_SetState(STATE_STANDBY);
                break;
                
            default:
                break;
        }
        
        // Update display
        Display_UpdateLCD();
    }
}

/* ===== STATE: SENSOR MONITOR ===== */

/**
 * @brief Sensor monitoring mode - Display sensor readings
 */
void Handle_SensorMonitor(void)
{
    // Check for timeout
    if (CheckTimeout(g_SystemContext.lastActivityTime, SCREEN_TIMEOUT_SEC * 1000)) {
        UART_Printf("[SENSOR] Timeout. Returning to menu.\r\n");
        StateMachine_SetState(STATE_ACTIVE_MENU);
        return;
    }
    
    // Auto-cycle through sensor screens
    static uint32_t lastScreenChange = 0;
    if (CheckTimeout(lastScreenChange, 2000)) { // Change every 2 seconds
        g_SystemContext.currentSensorScreen = 
            (g_SystemContext.currentSensorScreen + 1) % 3;
        lastScreenChange = GetSystemTick();
    }
    
    // Get keypad input
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        g_SystemContext.lastActivityTime = GetSystemTick();
        
        switch (key) {
            case KEY_UP:        // '2' - Previous screen
                if (g_SystemContext.currentSensorScreen > 0) {
                    g_SystemContext.currentSensorScreen--;
                }
                break;
                
            case KEY_DOWN:      // '8' - Next screen
                g_SystemContext.currentSensorScreen = 
                    (g_SystemContext.currentSensorScreen + 1) % 3;
                break;
                
            case KEY_BACK:      // '*'
                UART_Printf("[SENSOR] Back to menu.\r\n");
                StateMachine_SetState(STATE_ACTIVE_MENU);
                return;
                
            case KEY_LOGOUT:    // '#'
                UART_Printf("[SENSOR] Logout.\r\n");
                StateMachine_SetState(STATE_STANDBY);
                return;
                
            default:
                break;
        }
    }
    
    // Send sensor data via UART
    static uint32_t lastUARTUpdate = 0;
    if (CheckTimeout(lastUARTUpdate, 1000)) {
        Sensors_SendUART();
        lastUARTUpdate = GetSystemTick();
    }

}

/* ===== STATE: CONTROL DEVICES ===== */

/**
 * @brief Device control mode - Control LEDs, Relays, Buzzer
 */
void Handle_ControlDevices(void)
{
    // Check for timeout
    if (CheckTimeout(g_SystemContext.lastActivityTime, SCREEN_TIMEOUT_SEC * 1000)) {
        UART_Printf("[CONTROL] Timeout. Returning to menu.\r\n");
        StateMachine_SetState(STATE_ACTIVE_MENU);
        return;
    }
    
    // Get keypad input
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        g_SystemContext.lastActivityTime = GetSystemTick();
        
        switch (key) {
            case KEY_UP:        // '2' - Previous item
                if (g_SystemContext.currentControlItem > 0) {
                    g_SystemContext.currentControlItem--;
                    UART_Printf("[CONTROL] Item UP: %d\r\n", g_SystemContext.currentControlItem);
                }
                break;
                
            case KEY_DOWN:      // '8' - Next item
                if (g_SystemContext.currentControlItem < CONTROL_MENU_MAX - 1) {
                    g_SystemContext.currentControlItem++;
                    UART_Printf("[CONTROL] Item DOWN: %d\r\n", g_SystemContext.currentControlItem);
                }
                break;
                
            case KEY_ENTER:     // '5' - Toggle selected device
                UART_Printf("[CONTROL] Toggling item: %d\r\n", g_SystemContext.currentControlItem);
                
                switch (g_SystemContext.currentControlItem) {
                    case CONTROL_LED_GREEN:
                        Device_ToggleLED(LED_GREEN_PIN);
                        break;
                    case CONTROL_LED_RED:
                        Device_ToggleLED(LED_RED_PIN);
                        break;
                    case CONTROL_LED_WHITE:
                        Device_ToggleLED(LED_WHITE_PIN);
                        break;
                    case CONTROL_RELAY1:
                        Device_ToggleRelay(RELAY1_PIN);
                        break;
                    case CONTROL_RELAY2:
                        Device_ToggleRelay(RELAY2_PIN);
                        break;
                    case CONTROL_RELAY3:
                        Device_ToggleRelay(RELAY3_PIN);
                        break;
                    case CONTROL_RELAY4:
                        Device_ToggleRelay(RELAY4_PIN);
                        break;
                    case CONTROL_BUZZER:
                        Device_PlayBuzzer(BEEP_SUCCESS);
                        break;
                    case CONTROL_LDR_AUTO:
                        g_DeviceStates.ldr_auto_mode = !g_DeviceStates.ldr_auto_mode;
                        UART_Printf("[CONTROL] LDR Auto Mode: %s\r\n", 
                                    g_DeviceStates.ldr_auto_mode ? "ON" : "OFF");
                        break;
                    default:
                        break;
                }
                
                Device_PlayBuzzer(BEEP_SUCCESS);
                Device_SendStatusUART();
                break;
                
            case KEY_BACK:      // '*'
                UART_Printf("[CONTROL] Back to menu.\r\n");
                StateMachine_SetState(STATE_ACTIVE_MENU);
                return;
                
            case KEY_LOGOUT:    // '#'
                UART_Printf("[CONTROL] Logout.\r\n");
                StateMachine_SetState(STATE_STANDBY);
                return;
                
            default:
                break;
        }
        
        // Update display
        Display_UpdateLCD();
    }
}

/* ===== STATE: SETTINGS ===== */

/**
 * @brief Settings mode - System configuration
 */
void Handle_Settings(void)
{
    // Check for timeout
    if (CheckTimeout(g_SystemContext.lastActivityTime, SCREEN_TIMEOUT_SEC * 1000)) {
        UART_Printf("[SETTINGS] Timeout. Returning to menu.\r\n");
        StateMachine_SetState(STATE_ACTIVE_MENU);
        return;
    }
    
    // Get keypad input
    char key = Keypad_GetKey();
    if (key != KEYPAD_NO_KEY) {
        g_SystemContext.lastActivityTime = GetSystemTick();
        
        switch (key) {
            case KEY_BACK:      // '*'
                UART_Printf("[SETTINGS] Back to menu.\r\n");
                StateMachine_SetState(STATE_ACTIVE_MENU);
                return;
                
            case KEY_LOGOUT:    // '#'
                UART_Printf("[SETTINGS] Logout.\r\n");
                StateMachine_SetState(STATE_STANDBY);
                return;
                
            default:
                // Settings options can be implemented here
                break;
        }
    }
}

/* ===== STATE: LOCKOUT ===== */

/**
 * @brief Lockout mode - Security lockout after failed attempts
 */
void Handle_Lockout(void)
{
    // Check if lockout period has ended
    if (GetSystemTick() >= g_SystemContext.lockoutEndTime) {
        UART_Printf("[LOCKOUT] Lockout period ended. Returning to STANDBY.\r\n");
        g_SystemContext.loginAttempts = 0;
        StateMachine_SetState(STATE_STANDBY);
        return;
    }
    
    // Blink red LED
    static uint32_t lastBlink = 0;
    if (CheckTimeout(lastBlink, 500)) {
        BSP_LED_Toggle(LED_RED_PIN);
        lastBlink = GetSystemTick();
    }
    
    // Display countdown
    uint32_t remainingTime = (g_SystemContext.lockoutEndTime - GetSystemTick()) / 1000;
    
    static uint32_t lastUARTUpdate = 0;
    if (CheckTimeout(lastUARTUpdate, 1000)) {
        UART_Printf("[LOCKOUT] Time remaining: %lu seconds\r\n", remainingTime);
        lastUARTUpdate = GetSystemTick();
    }
}



/**
 * @brief Error state - System error with auto-recovery
 */
void Handle_Error(void)
{
    // Fast blink Red + White LEDs to indicate error
    static uint32_t lastBlink = 0;
    if (CheckTimeout(lastBlink, ERROR_BLINK_INTERVAL_MS)) {
        BSP_LED_Toggle(LED_RED_PIN);
        BSP_LED_Toggle(LED_WHITE_PIN);
        lastBlink = GetSystemTick();
    }
    
    // Check if recovery time has elapsed
    if (GetSystemTick() >= g_SystemContext.errorRecoveryTime) {
        UART_Printf("[ERROR] Recovery timeout reached. Attempting restart.\r\n");
        System_ClearError();
        return;
    }
    
    // Display countdown on LCD
    static uint32_t lastUpdate = 0;
    if (CheckTimeout(lastUpdate, 1000)) {
        uint32_t remainingTime = (g_SystemContext.errorRecoveryTime - GetSystemTick()) / 1000;
        
        char line2[20];
        snprintf(line2, 20, " Restart in: %2lus", remainingTime);
        BSP_LCD_SetCursor(1, 0);
        BSP_LCD_PrintString(line2);
        
        UART_Printf("[ERROR] Auto-restart in %lu seconds\r\n", remainingTime);
        lastUpdate = GetSystemTick();
    }
    
    // Allow manual reset via keypad '#' key
    char key = Keypad_GetKey();
    if (key == '#') {
        UART_Printf("[ERROR] Manual reset requested via keypad.\r\n");
        System_ClearError();
        return;
    }
}

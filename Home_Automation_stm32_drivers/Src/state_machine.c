/*
 * state_machine.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: State Machine Implementation - Core Logic
 */

#include <bsp_ir_sensors.h>
#include "state_machine.h"
#include "bsp_init.h"
#include "bsp_led.h"
#include "bsp_buzzer.h"
#include "bsp_relay.h"
#include "bsp_ldr.h"
#include "bsp_i2c_oled.h"
#include "bsp_lcd.h"
#include "bsp_keypad.h"
#include "bsp_uart2_debug.h"
#include "bsp_delay.h"
#include <string.h>
#include <stdio.h>

/* ===== GLOBAL VARIABLE DEFINITIONS ===== */
SystemContext_t g_SystemContext;
SensorData_t g_SensorData;
DeviceStates_t g_DeviceStates;

/* ===== PRIVATE VARIABLES ===== */
static uint32_t g_SystemTickCounter = 0;

/* ===== INITIALIZATION ===== */

/**
 * @brief Initialize the state machine and all subsystems
 */
void StateMachine_Init(void)
{
    // Initialize all BSP components
    app_init();
    
    // Clear all structures
    memset(&g_SystemContext, 0, sizeof(g_SystemContext));
    memset(&g_SensorData, 0, sizeof(g_SensorData));
    memset(&g_DeviceStates, 0, sizeof(g_DeviceStates));
    
    // Set initial state
    g_SystemContext.currentState = STATE_STANDBY;
    g_SystemContext.previousState = STATE_STANDBY;
    g_SystemContext.currentMenu = MENU_MAIN;
    g_SystemContext.currentSensorScreen = SENSOR_SCREEN_LDR;
    g_SystemContext.currentControlItem = CONTROL_LED_GREEN;
    
    // Initial display setup
    Display_ClearAll();
    
    // OLED Welcome Message
    BSP_OLED_Clear();
    BSP_OLED_PrintString(10, 0, "STM32 System");
    BSP_OLED_PrintString(0, 16, "Home Automation");
    BSP_OLED_PrintString(20, 32, "v1.0");
    BSP_OLED_PrintString(0, 48, "Initializing...");
    BSP_OLED_Update();
    
    // LCD Welcome
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString("  System Boot  ");
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString("  Please Wait  ");
    
    BSP_Delay_1s();
    
    // Turn on White LED (dim) for standby
    BSP_LED_On(LED_WHITE_PIN);
    
    // Success beep
    Device_PlayBuzzer(BEEP_SUCCESS);
    
    UART_Printf("\r\n");
    UART_Printf("========================================\r\n");
    UART_Printf("  STM32F446 Home Automation System\r\n");
    UART_Printf("  State Machine v1.0\r\n");
    UART_Printf("  Author: Rahul B.\r\n");
    UART_Printf("========================================\r\n");
    UART_Printf("[INIT] System initialization complete\r\n");
    UART_Printf("[INIT] Entering STANDBY mode\r\n");
    UART_Printf("\r\n");
    
    // Update displays for standby
    Display_UpdateOLED();
    Display_UpdateLCD();
    BSP_Delay_1s();
}

/* ===== CORE STATE MACHINE ===== */

/**
 * @brief Main state machine execution - Call this in main loop
 */

void StateMachine_Run(void)
{
    // Increment system tick
    g_SystemTickCounter++;
    
    // Update sensors periodically (with error handling)
    if (CheckTimeout(g_SystemContext.lastSensorUpdate, TEMP_UPDATE_INTERVAL)) {
        // Try to update sensors, report error if it fails
        if (!Sensors_Update()) {
            System_ReportError(ERROR_SENSOR_FAULT);
            return;
        }
        g_SystemContext.lastSensorUpdate = GetSystemTick();
    }
    
    // State Machine Logic
    switch (g_SystemContext.currentState) {
        case STATE_STANDBY:
            Handle_Standby();
            break;
            
        case STATE_AUTHENTICATING:
            Handle_Authentication();
            break;
            
        case STATE_ACTIVE_MENU:
            Handle_ActiveMenu();
            break;
            
        case STATE_LOCKOUT:
            Handle_Lockout();
            break;
            
        case STATE_ERROR:
            Handle_Error();
            break;
            
        default:
            // Invalid state - report error
            UART_Printf("[CRITICAL] Invalid state detected: %d\r\n", g_SystemContext.currentState);
            System_ReportError(ERROR_UNKNOWN);
            break;
    }
    
    // Small delay to prevent CPU hogging
    BSP_Delay_ms(10);
}
/* ===== UTILITY FUNCTIONS ===== */

/**
 * @brief Get current system tick in milliseconds
 */
uint32_t GetSystemTick(void)
{
    return g_SystemTickCounter;
}

/**
 * @brief Check if timeout has elapsed
 */
bool CheckTimeout(uint32_t lastTime, uint32_t interval)
{
    return (GetSystemTick() - lastTime) >= interval;
}

/**
 * @brief Convert LDR raw value to percentage
 */
uint8_t LDR_ToPercentage(uint16_t raw_value)
{
    // 12-bit ADC: 0-4095
    // Invert: High resistance (dark) = low ADC value
    return (uint8_t)((4095 - raw_value) * 100 / 4095);
}

/**
 * @brief Change system state with logging
 */
/**
 * @brief Change system state with logging
 */
void StateMachine_SetState(SystemState_t newState)
{
    if (g_SystemContext.currentState == newState) {
        return; // Already in this state
    }
    
    // Log state transition
    const char* stateNames[] = {
        "STANDBY", "AUTHENTICATING", "ACTIVE_MENU", 
        "LOCKOUT", "ERROR"
    };
    
    UART_Printf("[STATE] %s -> %s\r\n", 
                stateNames[g_SystemContext.currentState], 
                stateNames[newState]);
    
    g_SystemContext.previousState = g_SystemContext.currentState;
    g_SystemContext.currentState = newState;
    
    // State entry actions
    switch (newState) {
        case STATE_STANDBY:
            BSP_LED_AllOff();
            BSP_LED_On(LED_WHITE_PIN);
            g_SystemContext.isAuthenticated = false;
            memset(g_SystemContext.currentUser, 0, USERNAME_LENGTH);
            Display_ClearAll();
            g_SystemContext.errorCount = 0;  // Reset error count on successful return to standby
            break;
            
        case STATE_AUTHENTICATING:
            Auth_Reset();
            BSP_LED_On(LED_WHITE_PIN);
            break;
            
        case STATE_ACTIVE_MENU:
            BSP_LED_On(LED_GREEN_PIN);
            Device_PlayBuzzer(BEEP_SUCCESS);
            g_SystemContext.menuCursor = 0;
            break;
            
        case STATE_LOCKOUT:
            BSP_LED_AllOff();
            BSP_LED_On(LED_RED_PIN);
            Device_PlayBuzzer(BEEP_ALARM);
            g_SystemContext.lockoutEndTime = GetSystemTick() + (LOCKOUT_TIME_SEC * 1000);
            break;
            
        case STATE_ERROR:
            BSP_LED_AllOff();
            Device_PlayBuzzer(BEEP_WARNING);
            g_SystemContext.errorRecoveryTime = GetSystemTick() + (ERROR_RECOVERY_TIME_SEC * 1000);
            UART_Printf("[ERROR] System error detected. Auto-restart in %ds\r\n", ERROR_RECOVERY_TIME_SEC);
            
            // Display error on LCD
            BSP_LCD_SetCursor(0, 0);
            BSP_LCD_PrintString("  SYSTEM ERROR  ");
            BSP_LCD_SetCursor(1, 0);
            BSP_LCD_PrintString(" Restarting...  ");
            
            // Display error on OLED
            BSP_OLED_Clear();
            BSP_OLED_PrintString(10, 0, "SYSTEM ERROR");
            BSP_OLED_PrintString(0, 16, "Auto-restart in");
            char msg[20];
            snprintf(msg, 20, "%d seconds", ERROR_RECOVERY_TIME_SEC);
            BSP_OLED_PrintString(20, 32, msg);
            BSP_OLED_Update();
            break;
            
        default:
            break;
    }
}

/**
 * @brief Report error and transition to ERROR state
 */
void System_ReportError(SystemError_t error)
{
    g_SystemContext.lastError = error;
    g_SystemContext.errorCount++;
    
    const char* errorNames[] = {
        "NONE", "SENSOR_FAULT", "DISPLAY_FAULT", "KEYPAD_FAULT",
        "MEMORY_FAULT", "WATCHDOG_RESET", "HARD_FAULT", "UNKNOWN"
    };
    
    UART_Printf("[ERROR] Error reported: %s (Count: %d)\r\n", 
                errorNames[error], g_SystemContext.errorCount);
    
    // If too many consecutive errors, enter permanent lockout
    if (g_SystemContext.errorCount >= 5) {
        UART_Printf("[CRITICAL] Too many errors! System halted.\r\n");
        BSP_LCD_SetCursor(0, 0);
        BSP_LCD_PrintString(" CRITICAL ERROR ");
        BSP_LCD_SetCursor(1, 0);
        BSP_LCD_PrintString(" SYSTEM HALTED  ");
        
        // Halt system - infinite loop with blinking red LED
        while(1) {
            BSP_LED_Toggle(LED_RED_PIN);
            BSP_Buzzer_Toggle();
            BSP_Delay_ms(100);
        }
    }
    
    StateMachine_SetState(STATE_ERROR);
}

/**
 * @brief Clear error and restart system
 */
void System_ClearError(void)
{
    UART_Printf("[ERROR] Clearing error. Restarting system.\r\n");
    g_SystemContext.lastError = ERROR_NONE;
    
    // Reinitialize critical subsystems
    Display_ClearAll();
    BSP_LED_AllOff();
    BSP_Buzzer_Off();
    
    // Perform health check
    if (System_CheckHealth()) {
        UART_Printf("[ERROR] Health check passed. Returning to STANDBY.\r\n");
        StateMachine_SetState(STATE_STANDBY);
    } else {
        UART_Printf("[ERROR] Health check failed. Re-entering ERROR state.\r\n");
        System_ReportError(ERROR_UNKNOWN);
    }
}

/**
 * @brief System health check
 * @return true if system is healthy, false otherwise
 */
bool System_CheckHealth(void)
{
    bool healthy = true;
    
    // Check 1: Verify UART is working
    UART_Printf("[HEALTH] Checking UART... ");
    UART_Printf("OK\r\n");
    
    // Check 2: Verify LCD is responding
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString("Health Check...");
    BSP_Delay_ms(100);
    UART_Printf("[HEALTH] LCD check OK\r\n");
    
    // Check 3: Verify OLED is responding
    BSP_OLED_Clear();
    BSP_OLED_PrintString(0, 0, "Health Check");
    BSP_OLED_Update();
    BSP_Delay_ms(100);
    UART_Printf("[HEALTH] OLED check OK\r\n");
    
    // Check 4: Verify keypad is responding
    char test_key = Keypad_GetKey();
    UART_Printf("[HEALTH] Keypad check OK\r\n");
    
    // Check 5: Verify sensors
    uint16_t ldr_test = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
    if (ldr_test == 0 || ldr_test == 4095) {
        UART_Printf("[HEALTH] WARNING: LDR reading suspicious (%u)\r\n", ldr_test);
    } else {
        UART_Printf("[HEALTH] Sensor check OK\r\n");
    }
    
    return healthy;
}

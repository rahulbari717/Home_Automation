/*
 * auth_menu.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Authentication and Menu Navigation Functions
 */

#include "state_machine.h"
#include "bsp_lcd.h"
#include "bsp_uart2_debug.h"
#include "bsp_buzzer.h"
#include <string.h>
#include <stdio.h>

/* ===== AUTHENTICATION ===== */

// Simple user database (for demonstration)
typedef struct {
    char username[USERNAME_LENGTH];
    char pin[PIN_LENGTH + 1];
} User_t;

static const User_t userDatabase[] = {
    {"Rahul", "1234"},
    {"Admin", "0000"},
    {"User1", "1111"},
    {"User2", "2222"},
    {"Guest", "9999"}
};

#define USER_COUNT (sizeof(userDatabase) / sizeof(User_t))

/**
 * @brief Reset authentication state
 */
void Auth_Reset(void)
{
    memset(g_SystemContext.pinBuffer, 0, PIN_LENGTH + 1);
    g_SystemContext.pinIndex = 0;
    
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString("  Enter PIN:    ");
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString("                ");
    
    UART_Printf("[AUTH] Ready for PIN entry.\r\n");
}

/**
 * @brief Process keypress during authentication
 */
void Auth_ProcessKeyPress(char key)
{
    // Handle special keys
    if (key == KEY_BACK || key == KEY_LOGOUT) {
        UART_Printf("[AUTH] Cancelled. Returning to STANDBY.\r\n");
        StateMachine_SetState(STATE_STANDBY);
        return;
    }
    
    // Only accept digits for PIN
    if (key >= '0' && key <= '9') {
        if (g_SystemContext.pinIndex < PIN_LENGTH) {
            g_SystemContext.pinBuffer[g_SystemContext.pinIndex] = key;
            g_SystemContext.pinIndex++;
            
            // Display asterisks on LCD
            BSP_LCD_SetCursor(1, 0);
            for (int i = 0; i < g_SystemContext.pinIndex; i++) {
                BSP_LCD_SendData('*');
            }
            
            UART_Printf("[AUTH] PIN digit entered. Length: %d\r\n", g_SystemContext.pinIndex);
            
            // Check if PIN is complete
            if (g_SystemContext.pinIndex == PIN_LENGTH) {
                g_SystemContext.pinBuffer[PIN_LENGTH] = '\0';
                UART_Printf("[AUTH] PIN complete: ****\r\n");
                
                // Verify PIN
                if (Auth_VerifyPIN(g_SystemContext.pinBuffer)) {
                    UART_Printf("[AUTH] Login successful! User: %s\r\n", 
                                g_SystemContext.currentUser);
                    g_SystemContext.isAuthenticated = true;
                    g_SystemContext.loginAttempts = 0;
                    
                    BSP_LCD_SetCursor(0, 0);
                    BSP_LCD_PrintString("  Welcome!      ");
                    BSP_LCD_SetCursor(1, 0);
                    BSP_LCD_PrintString(g_SystemContext.currentUser);
                    BSP_Delay_ms(1000);
                    
                    StateMachine_SetState(STATE_ACTIVE_MENU);
                } else {
                    g_SystemContext.loginAttempts++;
                    UART_Printf("[AUTH] Invalid PIN. Attempts: %d/%d\r\n", 
                                g_SystemContext.loginAttempts, MAX_LOGIN_ATTEMPTS);
                    
                    BSP_LCD_SetCursor(0, 0);
                    BSP_LCD_PrintString(" Invalid PIN!   ");
                    BSP_LCD_SetCursor(1, 0);
                    char msg[17];
                    snprintf(msg, 17, "Attempts: %d/%d", 
                             g_SystemContext.loginAttempts, MAX_LOGIN_ATTEMPTS);
                    BSP_LCD_PrintString(msg);
                    
                    Device_PlayBuzzer(BEEP_ERROR);
                    BSP_Delay_ms(1500);
                    
                    if (g_SystemContext.loginAttempts >= MAX_LOGIN_ATTEMPTS) {
                        UART_Printf("[AUTH] Max attempts reached. LOCKOUT!\r\n");
                        StateMachine_SetState(STATE_LOCKOUT);
                    } else {
                        Auth_Reset();
                    }
                }
            }
        }
    }
}

/**
 * @brief Verify PIN against user database
 */
bool Auth_VerifyPIN(const char* pin)
{
    for (int i = 0; i < USER_COUNT; i++) {
        if (strcmp(pin, userDatabase[i].pin) == 0) {
            // PIN matched - copy username
            strncpy(g_SystemContext.currentUser, 
                    userDatabase[i].username, 
                    USERNAME_LENGTH - 1);
            g_SystemContext.currentUser[USERNAME_LENGTH - 1] = '\0';
            return true;
        }
    }
    
    // Check master PIN
    if (strcmp(pin, MASTER_PIN) == 0) {
        strncpy(g_SystemContext.currentUser, "Master", USERNAME_LENGTH - 1);
        return true;
    }
    
    return false;
}

/* ===== MENU NAVIGATION ===== */

// Menu text definitions
static const char* mainMenuItems[] = {
    "1.Sensor Monitor",
    "2.Device Control",
    "3.Settings      ",
    "4.Logout        "
};

static const char* controlMenuItems[] = {
    "LED Green",
    "LED Red  ",
    "LED White",
    "Relay 1  ",
    "Relay 2  ",
    "Relay 3  ",
    "Relay 4  ",
    "Buzzer   ",
    "LDR Auto "
};

/**
 * @brief Display current menu on LCD
 */
void Menu_Display(void)
{
    char line1[17];
    char line2[17];
    
    if (g_SystemContext.currentState == STATE_ACTIVE_MENU) {
        // Main Menu
        int topItem = g_SystemContext.menuCursor;
        int bottomItem = (g_SystemContext.menuCursor + 1) % 4;
        
        snprintf(line1, 17, ">%s", mainMenuItems[topItem]);
        snprintf(line2, 17, " %s", mainMenuItems[bottomItem]);
        
    } else if (g_SystemContext.currentState == STATE_CONTROL_DEVICES) {
        // Control Menu
        uint8_t item = g_SystemContext.currentControlItem;
        
        // Get device state
        bool state = false;
        switch (item) {
            case CONTROL_LED_GREEN: state = g_DeviceStates.led_green; break;
            case CONTROL_LED_RED: state = g_DeviceStates.led_red; break;
            case CONTROL_LED_WHITE: state = g_DeviceStates.led_white; break;
            case CONTROL_RELAY1: state = g_DeviceStates.relay1; break;
            case CONTROL_RELAY2: state = g_DeviceStates.relay2; break;
            case CONTROL_RELAY3: state = g_DeviceStates.relay3; break;
            case CONTROL_RELAY4: state = g_DeviceStates.relay4; break;
            case CONTROL_BUZZER: state = g_DeviceStates.buzzer_active; break;
            case CONTROL_LDR_AUTO: state = g_DeviceStates.ldr_auto_mode; break;
            default: break;
        }
        
        snprintf(line1, 17, ">%s:%s", 
                 controlMenuItems[item], 
                 state ? "ON " : "OFF");
        
        // Show next item if available
        if (item < CONTROL_MENU_MAX - 1) {
            uint8_t nextItem = item + 1;
            bool nextState = false;
            switch (nextItem) {
                case CONTROL_LED_GREEN: nextState = g_DeviceStates.led_green; break;
                case CONTROL_LED_RED: nextState = g_DeviceStates.led_red; break;
                case CONTROL_LED_WHITE: nextState = g_DeviceStates.led_white; break;
                case CONTROL_RELAY1: nextState = g_DeviceStates.relay1; break;
                case CONTROL_RELAY2: nextState = g_DeviceStates.relay2; break;
                case CONTROL_RELAY3: nextState = g_DeviceStates.relay3; break;
                case CONTROL_RELAY4: nextState = g_DeviceStates.relay4; break;
                case CONTROL_BUZZER: nextState = g_DeviceStates.buzzer_active; break;
                case CONTROL_LDR_AUTO: nextState = g_DeviceStates.ldr_auto_mode; break;
                default: break;
            }
            snprintf(line2, 17, " %s:%s", 
                     controlMenuItems[nextItem], 
                     nextState ? "ON " : "OFF");
        } else {
            snprintf(line2, 17, "                ");
        }
    }
    
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString(line1);
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString(line2);
}

/**
 * @brief Process menu selection
 */
void Menu_ProcessSelection(void)
{
    switch (g_SystemContext.menuCursor) {
        case 0: // Sensor Monitor
            UART_Printf("[MENU] Opening Sensor Monitor\r\n");
            StateMachine_SetState(STATE_SENSOR_MONITOR);
            break;
            
        case 1: // Device Control
            UART_Printf("[MENU] Opening Device Control\r\n");
            g_SystemContext.currentControlItem = CONTROL_LED_GREEN;
            StateMachine_SetState(STATE_CONTROL_DEVICES);
            break;
            
        case 2: // Settings
            UART_Printf("[MENU] Opening Settings\r\n");
            StateMachine_SetState(STATE_SETTINGS);
            break;
            
        case 3: // Logout
            UART_Printf("[MENU] Logging out\r\n");
            StateMachine_SetState(STATE_STANDBY);
            break;
            
        default:
            break;
    }
}
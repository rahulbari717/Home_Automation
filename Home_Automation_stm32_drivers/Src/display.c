/*
 * display.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Display Management Functions (OLED & LCD)
 */

#include "state_machine.h"
#include "bsp_lcd.h"
#include "bsp_i2c_oled.h"
#include <stdio.h>
#include <string.h>

/* ===== DISPLAY UPDATES ===== */

/**
 * @brief Update OLED display based on current state
 */
void Display_UpdateOLED(void)
{
    // Always show state information on OLED
    Sensors_DisplayOnOLED();
}

/**
 * @brief Update LCD display based on current state
 */
void Display_UpdateLCD(void)
{
    char line1[20];
    char line2[25];

    switch (g_SystemContext.currentState) {
        case STATE_STANDBY:
            BSP_LCD_SetCursor(0, 0);
            BSP_LCD_PrintString(" System Ready   ");
            BSP_LCD_SetCursor(1, 0);
            BSP_LCD_PrintString(" Press Any Key  ");
            break;

        case STATE_AUTHENTICATING:
            // Handled by Auth_ProcessKeyPress
            break;

        case STATE_ACTIVE_MENU:
            Menu_Display();
            break;

        case STATE_SENSOR_MONITOR:
            Sensors_DisplayOnLCD();
            break;

        case STATE_CONTROL_DEVICES:
            Menu_Display();
            break;

        case STATE_SETTINGS:
            BSP_LCD_SetCursor(0, 0);
            BSP_LCD_PrintString("   Settings     ");
            BSP_LCD_SetCursor(1, 0);
            BSP_LCD_PrintString(" *=Back #=Logout");
            break;

        case STATE_LOCKOUT:
            uint32_t remainingTime = (g_SystemContext.lockoutEndTime - GetSystemTick()) / 1000;
            snprintf(line1, 20, "  LOCKED OUT!   ");
            snprintf(line2, 25, "   Wait: %2lus   ", remainingTime);
            BSP_LCD_SetCursor(0, 0);
            BSP_LCD_PrintString(line1);
            BSP_LCD_SetCursor(1, 0);
            BSP_LCD_PrintString(line2);
            break;

        default:
            BSP_LCD_SetCursor(0, 0);
            BSP_LCD_PrintString("  Unknown State ");
            BSP_LCD_SetCursor(1, 0);
            BSP_LCD_PrintString("                ");
            break;
    }
}

/**
 * @brief Clear all displays
 */
void Display_ClearAll(void)
{
    // Clear LCD
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);

    // Clear OLED
    BSP_OLED_Clear();
    BSP_OLED_Update();
}

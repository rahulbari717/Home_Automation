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
#include "bsp_delay.h"
#include "bsp_uart2_debug.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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

/**
 * @brief Helper function to clear LCD and print two lines
 */
void update_lcd_display(const char *line1, const char *line2) {
    BSP_LCD_SendCommand(LCD_CMD_CLEAR);
    BSP_Delay_ms(2); // Wait for clear to finish
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString((char*)line1);
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString((char*)line2);
}

/**
 * @brief Unified Debug: Outputs a message to UART, LCD (Line 1), and OLED.
 */
void print_Log(const char *format, ...) {
    char buffer[32];
    va_list args;

    // 1. Format the string
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 2. Output to UART
    UART_Printf("[DEBUG] %s\r\n", buffer);

    // 3. Output to LCD (Row 0)
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString("                "); // Manual clear line
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString(buffer);

    // 4. Output to OLED (Row 20)
    OLED_Printf(0, 20, "> %-16s", buffer); // Use padding to overwrite old text
    BSP_OLED_Update();
}


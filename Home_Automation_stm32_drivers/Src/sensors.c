/*
 * sensors.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Sensor Reading and Management Functions
 */

#include "state_machine.h"
#include "bsp_ldr.h"
#include "bsp_uart2_debug.h"
#include <stdio.h>

#include "bsp_lcd.h"
#include "bsp_i2c_oled.h"
#include "bsp_uart2_debug.h"

#include <stdint.h>
#include <stdbool.h>


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
* @brief Display sensor data on LCD based on current screen
*/
void Sensors_DisplayOnLCD(void)
{
   char line1[30];
   char line2[30];

   switch (g_SystemContext.currentSensorScreen) {
       case SENSOR_SCREEN_LDR:
           snprintf(line1, 20, "LDR1:%4u (%2u%%)",
                    g_SensorData.ldr1_value,
                    LDR_ToPercentage(g_SensorData.ldr1_value));
           snprintf(line2, 20, "LDR2:%4u (%2u%%)",
                    g_SensorData.ldr2_value,
                    LDR_ToPercentage(g_SensorData.ldr2_value));
           break;

       default:
           snprintf(line1, 20, "Sensor Screen");
           snprintf(line2, 20, "Select Mode");
           break;
   }

   BSP_LCD_SetCursor(0, 0);
   BSP_LCD_PrintString(line1);
   BSP_LCD_SetCursor(1, 0);
   BSP_LCD_PrintString(line2);
}
/**
* @brief Display system state on OLED
*/
void Sensors_DisplayOnOLED(void)
{
   BSP_OLED_Clear();

   // Display current state
   const char* stateNames[] = {
       "STANDBY", "AUTH", "MENU",
       "SENSORS", "CONTROL", "SETTINGS", "LOCKOUT"
   };

   char buffer[32];

   // Line 0: State
   snprintf(buffer, 32, "STATE: %s", stateNames[g_SystemContext.currentState]);
   
   BSP_OLED_PrintString(0, 0, buffer);

   // Line 2: User
   if (g_SystemContext.isAuthenticated) {
       snprintf(buffer, 32, "User: %s", g_SystemContext.currentUser);
   } else {
       snprintf(buffer, 32, "User: [None]");
   }
   BSP_OLED_PrintString(0, 16, buffer);

   BSP_OLED_PrintString(0, 32, buffer);

   BSP_OLED_Update();
}

/*
 * sensors.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: Sensor Reading and Management Functions
 */

#include <bsp_ir_sensors.h>
#include "state_machine.h"
#include "bsp_ldr.h"
#include "bsp_uart2_debug.h"
#include <stdio.h>

/* ===== SENSOR UPDATE ===== */

/**
 * @brief Update all sensor readings
 */
bool Sensors_Update(void) 
{
    // Read LDR sensors
    g_SensorData.ldr1_value = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
    g_SensorData.ldr2_value = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);
     
    // Read digital sensors
    g_SensorData.ir1_detected = BSP_Sensor_Read_IR1();
    g_SensorData.ir2_detected = BSP_Sensor_Read_IR2();
    
    // Update timestamp
    g_SensorData.lastUpdateTime = GetSystemTick();
    return true;
}

/* ===== UART OUTPUT ===== */

/**
 * @brief Send sensor data via UART
 */
void Sensors_SendUART(void)
{
    UART_Printf("[SENSOR] === Sensor Readings ===\r\n");
    UART_Printf("[SENSOR] LDR1: %4u (%3u%%) | LDR2: %4u (%3u%%)\r\n",
                g_SensorData.ldr1_value, 
                LDR_ToPercentage(g_SensorData.ldr1_value),
                g_SensorData.ldr2_value,
                LDR_ToPercentage(g_SensorData.ldr2_value));

    UART_Printf("[SENSOR] IR1: %s | IR2: %s\r\n",
                g_SensorData.ir1_detected ? "YES" : "NO",
                g_SensorData.ir2_detected ? "YES" : "NO");
    
    UART_Printf("[SENSOR] ========================\r\n");
}

/**
 * @brief Display sensor data on LCD based on current screen
 */
void Sensors_DisplayOnLCD(void)
{
    char line1[20];
    char line2[20];
    
    switch (g_SystemContext.currentSensorScreen) {
        case SENSOR_SCREEN_LDR:
            snprintf(line1, 20, "LDR1:%4u (%2u%%)",
                     g_SensorData.ldr1_value,
                     LDR_ToPercentage(g_SensorData.ldr1_value));
            snprintf(line2, 20, "LDR2:%4u (%2u%%)",
                     g_SensorData.ldr2_value,
                     LDR_ToPercentage(g_SensorData.ldr2_value));
            break;

        case SENSOR_SCREEN_MOTION:
            // Display IR status only
            snprintf(line1, 20, "IR Monitoring");
            snprintf(line2, 20, "IR1:%s  IR2:%s",
                     g_SensorData.ir1_detected ? "Y" : "N",
                     g_SensorData.ir2_detected ? "Y" : "N");
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

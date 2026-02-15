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
#include "state_machine.h"
#include <stdint.h>
#include <stdbool.h>

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
   char line1[30];
   char line2[30];

   if (g_SystemContext.currentState == STATE_ACTIVE_MENU) {
       // Main Menu
       int topItem = g_SystemContext.menuCursor;
       int bottomItem = (g_SystemContext.menuCursor + 1) % 4;

       snprintf(line1, 30, ">%s", mainMenuItems[topItem]);
       snprintf(line2, 30, " %s", mainMenuItems[bottomItem]);

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

/*
 * state_machine.c
 *
 * Created on: Jan 28, 2026
 * Author: Rahul B.
 * Description: State Machine Implementation - Core Logic
 */

#include <stdbool.h>
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
#include <stdint.h>

/* ===== GLOBAL VARIABLE DEFINITIONS ===== */
SystemContext_t g_SystemContext;
SensorData_t g_SensorData;
DeviceStates_t g_DeviceStates;

extern volatile bool pc13_event;
extern volatile bool pc8_intrusion_event;
extern volatile bool pc6_intrusion_event;
extern volatile uint32_t pc13_event_time;

void Process_Intrusion_Events(void);

///* ===== PRIVATE VARIABLES ===== */
static uint32_t g_SystemTickCounter = 0;

void SysTick_Handler(void)
{
   g_SystemTickCounter++;
}

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
//   BSP_OLED_Clear();
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
   UART_Printf("========================================\r\n");
   UART_Printf("\r\n");

   // Update displays for standby
//   Display_UpdateOLED();
//   Display_UpdateLCD();
   BSP_Delay_1s();
}

/**
 * @brief Main state machine execution - Call this in main loop
 */
void StateMachine_Run(void)
{
    // Process intrusion events
//    Process_Intrusion_Events();

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

        case STATE_SENSOR_MONITOR:
            Handle_SensorMonitor();
            break;

        case STATE_CONTROL_DEVICES:
            Handle_ControlDevices();
            break;

        case STATE_LOCKOUT:
            Handle_Lockout();
            break;

        default:
            // Invalid state - return to standby
            UART_Printf("[CRITICAL] Invalid state detected: %d\r\n", g_SystemContext.currentState);
            g_SystemContext.currentState = STATE_STANDBY;
            break;
    }

    // Small delay to prevent CPU hogging
    BSP_Delay_ms(20);
}

void Process_Intrusion_Events(void)
{
    static uint32_t last_intrusion_time = 0;
    static bool intrusion_active = false;

    bool pc6_triggered = false;
    bool pc8_triggered = false;

    /* --- Atomic flag copy --- */
    __asm volatile ("cpsid i" ::: "memory");

    pc6_triggered = pc6_intrusion_event;
    pc8_triggered = pc8_intrusion_event;
    pc6_intrusion_event = false;
    pc8_intrusion_event = false;

    __asm volatile ("cpsie i" ::: "memory");
    uint32_t current_time = GetSystemTick();

    /* --- Intrusion detected --- */
    if ((pc6_triggered || pc8_triggered) &&
        CheckTimeout(last_intrusion_time, 2000))
    {
        UART_Printf("[INTRUSION] Perimeter Breach!\r\n");

        BSP_LED_On(LED_RED_PIN);
        Device_PlayBuzzer(BEEP_WARNING);
        update_lcd_display("INTRUSION!", "Check Perimeter");

        last_intrusion_time = current_time;
        intrusion_active = true;
    }

    /* --- Auto clear LED after 1 sec --- */
    if (intrusion_active &&
        CheckTimeout(last_intrusion_time, 1000))
    {
        BSP_LED_Off(LED_RED_PIN);
        intrusion_active = false;
    }
}

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

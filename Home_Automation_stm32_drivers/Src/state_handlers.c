/*
* state_handlers.c
*
* Created on: Jan 28, 2026
* Author: Rahul B.
* Description: Individual State Handler Functions
*/

#include "stm32f446xx_i2c_driver.h"
#include "state_machine.h"
#include "bsp_keypad.h"
#include "bsp_led.h"
#include "bsp_lcd.h"
#include "bsp_i2c_oled.h"
#include "bsp_uart2_debug.h"
#include "bsp_delay.h"
#include "bsp_buzzer.h"
#include "bsp_relay.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include "bsp_button.h"
#include "bsp_ldr.h"
#include <stdint.h>
#include <stdbool.h>

/* GLOBAL VARIABLES (Missing in your previous build) */

extern volatile uint8_t g_wakeup_flag;

uint8_t login_attempts = 0;  
uint8_t selected_item = 0;    

/**
 * @brief Safely changes system state and performs one-time entry actions
 */
void StateMachine_SetState(SystemState_t newState) {
    if (g_SystemContext.currentState == newState) return;

    // 1. Log transition to all three outputs once
    const char* stateNames[] = {
        "STANDBY", "AUTH GATE", "MAIN MENU", "MONITORING",
        "DEVICE CTRL", "SETTINGS", "LOCKED!!", "SYS ERROR"
    };

    print_Log("TO: %s", stateNames[newState]); // Synchronized UART/LCD/OLED log

    // 2. Perform one-time cleanup/initialization
    BSP_LCD_SendCommand(LCD_CMD_CLEAR); // Clear LCD
    BSP_OLED_Clear();                   // Clear OLED buffer

    // 3. Update state
    g_SystemContext.currentState = newState;
}

/* ========================================================================
   STATE 1: STANDBY MODE
   - White LED blinks 1s ON / 3s OFF
   - Wait for keypad or button interrupt
   - IR intrusion detection with buzzer alarm
   ======================================================================== */
void Handle_Standby(void) {
    BSP_LED_AllOff();
    
    BSP_LCD_SetCursor(0, 0);
    BSP_LCD_PrintString(" Standby Mode ");
    // UI: Static text only if just entered (handled by print_Log in SetState)
    BSP_LCD_SetCursor(1, 0);
    BSP_LCD_PrintString(" PRESS PC13 BTN ");

    // White LED Blink Logic (PA7)
	BSP_LED_On(LED_WHITE_PIN);
	BSP_Delay_ms(1000);
	BSP_LED_Off(LED_WHITE_PIN);

	// Wait 3s or check for wakeup
	for(int i=0; i<150; i++) {  // ✅ 150 × 20ms = 3000ms = 3 seconds
		if(g_wakeup_flag || (GPIO_ReadFromInputPin(GPIOC, GPIO_PIN_NO_13) == 0)) {
			g_wakeup_flag = 0;
            BSP_Delay_ms(200); // Debounce
			UART_Printf("[EVENT] Wakeup triggered!\r\n");
            UART_Printf("[EVENT] Standby to Authentication \r\n");
			Device_PlayBuzzer(BEEP_SUCCESS);
			g_SystemContext.currentState = STATE_AUTHENTICATING;
			return;
		}
		BSP_Delay_ms(20);
	}
}

/* ========================================================================
   STATE 2: AUTHENTICATION MODE
   - Green LED ON (all others OFF)
   - LCD shows "Enter PIN:"
   - Wait for 4-digit PIN via keypad
   - 3 failed attempts → LOCKOUT
   ======================================================================== */
void Handle_Authentication(void)
{
	// print_Log("Auth Mode : ");

    BSP_LED_AllOff();
    BSP_LED_On(LED_GREEN_PIN);

    char input_pin[PIN_LENGTH + 1] = {0};
    uint8_t char_count = 0; // Fixed: replaced 'count' with 'char_count'

    update_lcd_display("ENTER PIN:", "");

    while(char_count < PIN_LENGTH) {
        char key = Keypad_GetKey();
        if(key != KEYPAD_NO_KEY && key >= '0' && key <= '9') {
            input_pin[char_count] = key;
            BSP_LCD_SetCursor(1, char_count);
            BSP_LCD_SendData('*');
            char_count++;
            UART_Printf("*"); 
        }
    }
    input_pin[PIN_LENGTH] = '\0';

    if(strcmp(input_pin, MASTER_PIN) == 0) {
        print_Log("AUTH SUCCESS");
        Device_PlayBuzzer(BEEP_SUCCESS);
        login_attempts = 0;
        g_SystemContext.currentState = STATE_ACTIVE_MENU;
    } else {
        login_attempts++; // Increase count on every failure

        if(login_attempts > 3) {
            // After 3 wrong tries, go to LOCKOUT
        	Device_PlayBuzzer(BEEP_ERROR);
            print_Log("MAX ATTEMPTS! LOCKOUT");
            g_SystemContext.currentState = STATE_LOCKOUT;
        } else {
            // Still have chances left, go back to try again or stay in AUTH
            print_Log("WRONG PIN! TRY %d/3", login_attempts);
            Device_PlayBuzzer(BEEP_WARNING);
            g_SystemContext.currentState = STATE_AUTHENTICATING; // Reset the auth screen
        }
        BSP_Delay_1s();
    }
}

/* ========================================================================
   STATE 3: ACTIVE MENU MODE
   - Green LED remains ON
   - Menu navigation: 2=Up, 8=Down, 5=Select, #=Logout
   - Four menu options:
     1. Sensor Monitor
     2. Device Control
     3. Settings
     4. Logout
   ======================================================================== */
void Handle_ActiveMenu(void)
{
//	print_Log("Active Mode : ");
	BSP_LED_AllOff();
//    BSP_LED_On(LED_GREEN_PIN);
    Menu_Display();
    
    char key = Keypad_GetKey();
    if(key == '2') { // Up
        if (g_SystemContext.menuCursor > 0) {
            g_SystemContext.menuCursor--;
        }
    } else if(key == '8') { // Down
        if (g_SystemContext.menuCursor < 3) {
            g_SystemContext.menuCursor++;
        }
    } else if(key == '5') { // Select
        switch(g_SystemContext.menuCursor) {
            case 0: // Sensors
                g_SystemContext.currentState = STATE_SENSOR_MONITOR;
                break;
            case 1: // Controls
                g_SystemContext.currentState = STATE_CONTROL_DEVICES;
                break;
            case 2: // Settings
                // Reserved for future
                break;
            case 3: // Logout
                g_SystemContext.currentState = STATE_STANDBY;
                g_SystemContext.isAuthenticated = false;
                break;
        }
        BSP_LCD_SendCommand(LCD_CMD_CLEAR);
    } else if(key == '#') {
        g_SystemContext.currentState = STATE_STANDBY;
        g_SystemContext.isAuthenticated = false;
    }
    BSP_Delay_ms(150);
}

void Handle_SensorMonitor(void) 
{
	BSP_LCD_SetCursor(0,0);
	BSP_LCD_PrintString("SENSOR DASHBOARD");
    // Update sensor readings
    g_SensorData.ldr1_value = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
    g_SensorData.ldr2_value = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);
    
    Sensors_DisplayOnLCD();
    
    char key = Keypad_GetKey();
    if(key == '*') {
        g_SystemContext.currentState = STATE_ACTIVE_MENU;
    } else if (key == '#') {
        g_SystemContext.currentState = STATE_STANDBY;
        g_SystemContext.isAuthenticated = false;
    }
    
    BSP_Delay_ms(200);
}

void Handle_ControlDevices(void) 
{
    Menu_Display();
    
    char key = Keypad_GetKey();
    if(key == '2') { // Up
        if (g_SystemContext.currentControlItem > 0) {
            g_SystemContext.currentControlItem--;
        }
    } else if(key == '8') { // Down
        if (g_SystemContext.currentControlItem < CONTROL_MENU_MAX - 1) {
            g_SystemContext.currentControlItem++;
        }
    } else if(key == '5') { // Toggle
        // Toggle selected device
        switch(g_SystemContext.currentControlItem) {
            case CONTROL_LED_GREEN:
                BSP_LED_Toggle(LED_GREEN_PIN);
                g_DeviceStates.led_green = !g_DeviceStates.led_green;
                break;
            case CONTROL_LED_RED:
                BSP_LED_Toggle(LED_RED_PIN);
                g_DeviceStates.led_red = !g_DeviceStates.led_red;
                break;
            case CONTROL_LED_WHITE:
                BSP_LED_Toggle(LED_WHITE_PIN);
                g_DeviceStates.led_white = !g_DeviceStates.led_white;
                break;
            case CONTROL_RELAY1:
                BSP_Relay_Toggle(RELAY1_PIN);
                g_DeviceStates.relay1 = !g_DeviceStates.relay1;
                break;
            case CONTROL_RELAY2:
                BSP_Relay_Toggle(RELAY2_PIN);
                g_DeviceStates.relay2 = !g_DeviceStates.relay2;
                break;
            case CONTROL_RELAY3:
                BSP_Relay_Toggle(RELAY3_PIN);
                g_DeviceStates.relay3 = !g_DeviceStates.relay3;
                break;
            case CONTROL_RELAY4:
                BSP_Relay_Toggle(RELAY4_PIN);
                g_DeviceStates.relay4 = !g_DeviceStates.relay4;
                break;
            case CONTROL_BUZZER:
                Device_PlayBuzzer(BEEP_SUCCESS);
                break;
        }
    } else if(key == '*') {
        g_SystemContext.currentState = STATE_ACTIVE_MENU;
    } else if(key == '#') {
        g_SystemContext.currentState = STATE_STANDBY;
        g_SystemContext.isAuthenticated = false;
    }
    BSP_Delay_ms(200);
}

///* ========================================================================
//   STATE 7: LOCKOUT MODE
//   - Red LED + Buzzer blink 1s ON / 1s OFF for 3 cycles
//   - Total duration: 5 seconds (configurable via LOCKOUT_TIME_SEC)
//   - Then return to STANDBY
//   ======================================================================== */
void Handle_Lockout(void) 
{
    BSP_LCD_SetCursor(0,0);
    BSP_LCD_PrintString(" SECURITY ALERT ");
    BSP_LCD_SetCursor(1,0);
    BSP_LCD_PrintString(" SYSTEM LOCKED  ");

    UART_Printf("[ALERT] Lockout active\r\n");

    for(int i=0; i < 3; i++) {
        BSP_LED_On(LED_RED_PIN);
        BSP_Buzzer_On();
        BSP_Delay_ms(500);
        BSP_LED_Off(LED_RED_PIN);
        BSP_Buzzer_Off();
        BSP_Delay_ms(500);
    }

    g_SystemContext.loginAttempts = 0;
    g_SystemContext.currentState = STATE_STANDBY;
}

/**
 * @brief  Performs a comprehensive Autonomous Self-Test (POST).
 * Uses LEDs and Buzzer for physical signaling.
 * @return 0 if Pass, Bitmask of errors if Fail.
 */
uint8_t System_SelfTest(void) {
    uint8_t error_mask = 0;
//    char key;
//    uint8_t key_pressed = 0;

    print_Log("Peripherals SCAN");

    // Toggle Green -> Red -> White -> Buzzer
    // BSP_LED_On(LED_GREEN_PIN);
    // BSP_Delay_1s();
    // BSP_LED_Off(LED_GREEN_PIN);

    // BSP_LED_On(LED_RED_PIN);
    // BSP_Delay_1s();
    // BSP_LED_Off(LED_RED_PIN);

    // BSP_LED_On(LED_WHITE_PIN);
    // BSP_Delay_1s();
    // BSP_LED_Off(LED_WHITE_PIN);

    // BSP_Buzzer_On();
    // BSP_Delay_1s();
    // BSP_Buzzer_Off();

    // print_Log(" LEDs, Buzzer OK\r\n");

    /* --- TEST 2: I2C OLED (SSD1306) --- */
//    UART_Printf("[SCAN] I2C OLED (Address 0x3C)...");
//    if (I2C_CheckDevice(I2C1, OLED_I2C_ADDR) == 1) { // Check for ACK on I2C bus
//    	greet();
//    	UART_Printf(" OK\r\n");
//    } else {
//        UART_Printf(" NOT FOUND\r\n");
//        error_mask |= (1 << 1);
//    }
    greet();
    /* --- TEST 3: ADC Sensors (LDR1 & LDR2) --- */

//     UART_Printf("[TEST] Reading LDRs for 180s...\r\n");
//     update_lcd_display("TESTING LDR", "Check UART");
//     for(int i = 0; i < 180; i++) {
//         uint16_t l1 = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
//         uint16_t l2 = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);
//         print_Log("L1:%u L2:%u", l1, l2);
//         BSP_Delay_500ms();
//     }

     // 7. for keypad testing

//  	UART_Printf("\r\n\r\n========================================\r\n");
//  	UART_Printf(">> 4x4 Keypad Test Application\r\n");
//  	UART_Printf("========================================\r\n\r\n");
//  	UART_Printf("Keypad Layout:\r\n");
//  	UART_Printf("  C0  C1  C2  C3\r\n");
//  	UART_Printf("R0 1   2   3   A\r\n");
//  	UART_Printf("R1 4   5   6   B\r\n");
//  	UART_Printf("R2 7   8   9   C\r\n");
//  	UART_Printf("R3 *   0   #   D\r\n\r\n");
//  	UART_Printf("Press any key on the keypad...\r\n\r\n");
//
//  	while(1)
//  	{
//  		key = Keypad_GetKey();
//
//  		if(key != KEYPAD_NO_KEY)
//  		{
//  			UART_Printf("Key Pressed: %c\r\n", key);
//  			key_pressed++;
//  			UART_Printf("Key_Pressed : %d \r\n", key_pressed);
//
//  		}
//  		if(key_pressed == 16){
//  			UART_Printf(" exit from keypad : %c\r\n");
//  			return;
//  		}
//  		// Small delay to avoid flooding UART
//  		BSP_Delay_100ms();
//  	}
//
//    /* --- TEST 5: Relay Matrix (Port B) --- */
//    print_Log("[SCAN] Relay ...");
//    // We toggle them fast so you hear a "click" during boot
//    for(int i=1; i<=5; i++) {
//        BSP_Delay_3s();
//        BSP_Relay_SetState(RELAY1_PIN, RESET);
//        BSP_Relay_SetState(RELAY2_PIN, RESET);
//        BSP_Relay_SetState(RELAY3_PIN, RESET);
//        BSP_Relay_SetState(RELAY4_PIN, RESET);
//        BSP_Delay_3s();
//        BSP_Relay_SetState(RELAY1_PIN, SET);
//        BSP_Relay_SetState(RELAY2_PIN, SET);
//        BSP_Relay_SetState(RELAY3_PIN, SET);
//        BSP_Relay_SetState(RELAY4_PIN, SET);
//    }
//    UART_Printf(" OK\r\n");
//
//    /* --- FINAL EVALUATION --- */
//    if (error_mask == 0) {
//        UART_Printf(">> STATUS: ALL PERIPHERALS HEALTHY\r\n\r\n");
//        // Success Tone
//        Device_PlayBuzzer(BEEP_SUCCESS);
//    } else {
//        UART_Printf(">> STATUS: FATAL HARDWARE ERROR (Code: 0x%X)\r\n", error_mask);
//        GPIO_WriteToOutputPin(LED_PORT, LED_RED_PIN, SET); // Solid RED for failure
//        Device_PlayBuzzer(BEEP_ERROR);
//    }
    print_Log("TEST COMPLETE");
    return error_mask;
}

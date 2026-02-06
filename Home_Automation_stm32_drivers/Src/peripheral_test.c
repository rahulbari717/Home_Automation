/*
 * peripheral_test.c
 *
 *  Created on: Jan 28, 2026
 *      Author: Rahul B.
 */

#include <bsp_ir_sensors.h>
#include "state_machine.h"
#include "bsp_uart2_debug.h"
#include "bsp_led.h"
#include "bsp_buzzer.h"
#include "bsp_lcd.h"
#include "bsp_i2c_oled.h"
#include "bsp_delay.h"
#include "bsp_button.h"
#include "bsp_ldr.h"

#include <stdio.h>

void Run_Complete_Peripheral_Test(void) {

	uint32_t start_tick = 0;
	char key;

    UART_Printf("\r\n=== STARTING PERIPHERAL TEST ===\r\n");

    // 1. LCD & OLED Status Update
    update_lcd_display("PERIPHERAL", "TESTING...");
    BSP_OLED_Clear();
    OLED_Printf(0, 0, "PERIPHERAL");
    OLED_Printf(0, 16, "TESTING...");
    BSP_OLED_Update();

    // Blocking 10 sec wait (using your 1s delay 60 times)
    for(int i = 0; i < 10; i++) BSP_Delay_1s();

    // 2. LEDs and Buzzer ON for 1 Minute
    UART_Printf("[TEST] LEDs and Buzzer: ON (60s)\r\n");

    update_lcd_display("LEDs, Buzzer ", "TESTING...");
    BSP_OLED_Clear();
    OLED_Printf(0, 0, "LEDs, Buzzer ");
    OLED_Printf(0, 16, "TESTING...");
    BSP_OLED_Update();

    BSP_LED_On(LED_RED_PIN);
    BSP_LED_On(LED_GREEN_PIN);
    BSP_LED_On(LED_WHITE_PIN);
    BSP_Buzzer_On();

    // Blocking 1-minute wait (using your 1s delay 60 times)
    for(int i = 0; i < 60; i++) BSP_Delay_1s();

    BSP_LED_AllOff();
    BSP_Buzzer_Off();
    BSP_OLED_Clear();
    OLED_Printf(0, 0, "LEDs, Buzzer ");
    OLED_Printf(0, 16, "Testing completed ...");
    BSP_OLED_Update();
    update_lcd_display("LEDs, Buzzer ", "TESTING completed ...");
    UART_Printf("[TEST] LEDs and Buzzer: TESTING completed \r\n");

    // Blocking 10 sec wait (using your 1s delay 60 times)
    for(int i = 0; i < 10; i++) BSP_Delay_1s();

    //  3. Button Testing (PC13)
    UART_Printf("[TEST] Press User Button (PC13) to test...\r\n");
    update_lcd_display("TESTING BUTTON", "Press PC13");

    // Test button for 10 seconds
    start_tick = GetSystemTick();
    while((GetSystemTick() - start_tick) < 1000000) { // Approx 10s based on your tick logic
        if(BSP_Button_Read()  == BSP_BUTTON_PRESSED) {
            BSP_LED_On(LED_GREEN_PIN);
            BSP_Buzzer_On();
        } else {
            BSP_LED_Off(LED_GREEN_PIN);
            BSP_Buzzer_Off();
        }
    }

    // 4. LDR Readings for 1 Minute
    UART_Printf("[TEST] Reading LDRs for 180s...\r\n");
    update_lcd_display("TESTING LDR", "Check UART");
    for(int i = 0; i < 180; i++) {
        uint16_t l1 = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);
        uint16_t l2 = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);
        UART_Printf("LDR1: %u | LDR2: %u\r\n", l1, l2);
        BSP_Delay_500ms();
    }

    // 5. IR Test (60 seconds)
    UART_Printf("[TEST] Testing Motion/IR Sensors...\r\n");
    update_lcd_display("TESTING MOTION", "Trigger Sensors");
    start_tick = GetSystemTick();

    while((GetSystemTick() - start_tick) < 6000000) {
        if(BSP_Sensor_Read_IR1()) {
            BSP_LED_On(LED_GREEN_PIN);
            BSP_Buzzer_Beep(900);
            UART_Printf("IR1 Detected!\r\n");
        }
        if(BSP_Sensor_Read_IR2()) {
            BSP_LED_On(LED_RED_PIN);
            BSP_Buzzer_Beep(900);
            UART_Printf("IR2 Detected!\r\n");
        }

        BSP_LED_Off(LED_GREEN_PIN);
        BSP_LED_Off(LED_RED_PIN);
        BSP_LED_Off(LED_WHITE_PIN);
    }

    // 7. for keypad testing

	UART_Printf("\r\n\r\n========================================\r\n");
	UART_Printf(">> 4x4 Keypad Test Application\r\n");
	UART_Printf("========================================\r\n\r\n");
	UART_Printf("Keypad Layout:\r\n");
	UART_Printf("  C0  C1  C2  C3\r\n");
	UART_Printf("R0 1   2   3   A\r\n");
	UART_Printf("R1 4   5   6   B\r\n");
	UART_Printf("R2 7   8   9   C\r\n");
	UART_Printf("R3 *   0   #   D\r\n\r\n");
	UART_Printf("Press any key on the keypad...\r\n\r\n");

	while(1)
	{
		key = Keypad_GetKey();

		if(key != KEYPAD_NO_KEY)
		{
			UART_Printf("Key Pressed: %c\r\n", key);
		}

		// Small delay to avoid flooding UART
		for(volatile uint32_t i = 0; i < 100000; i++);
	}

	// 8. relay testing
	UART_Printf("[TEST] Reading RELAYs for 60s...\r\n");
	update_lcd_display("TESTING RELAY", "Check UART");
	for(int i = 0; i < 60; i++) {
		BSP_Relay_SetState(RELAY1_PIN, GPIO_PIN_SET);
		BSP_Relay_SetState(RELAY2_PIN, GPIO_PIN_SET);
		BSP_Relay_SetState(RELAY3_PIN, GPIO_PIN_RESET);
		BSP_Relay_SetState(RELAY4_PIN, GPIO_PIN_RESET);
		BSP_Delay_3s();
		BSP_Relay_SetState(RELAY1_PIN, GPIO_PIN_RESET);
		BSP_Relay_SetState(RELAY2_PIN, GPIO_PIN_RESET);
		BSP_Relay_SetState(RELAY3_PIN, GPIO_PIN_SET);
		BSP_Relay_SetState(RELAY4_PIN, GPIO_PIN_SET);
		BSP_Delay_3s();
	}
    UART_Printf("=== TEST COMPLETE ===\r\n");
    update_lcd_display("TEST COMPLETE", "Entering Standby");
    BSP_Delay_ms(2000);
}

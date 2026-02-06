/*
 * bsp_init.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Board Support Package - System Initialization
 */

#include <bsp_ir_sensors.h>
#include "bsp_init.h"
#include "bsp_led.h"
#include "bsp_buzzer.h"
#include "bsp_relay.h"
#include "bsp_ldr.h"
#include "bsp_i2c_oled.h"
#include "bsp_lcd.h"
#include "bsp_keypad.h"
#include "bsp_delay.h"
#include "bsp_button.h"
#include "bsp_uart2_debug.h"

void app_init(void) {
	/* 1. System Core (Clock & Basic Timing) */
	    BSP_Delay_Init();

	    /* 2. Debugging (UART2) */
	    USART2_GPIOInit();
	    USART2_Init();

	    /* 3. User Interface */
	    BSP_LED_Init();
	    BSP_Buzzer_Init();
	    BSP_Button_Init();
	    Keypad_Init();

	    /* 4. Displays */
	    BSP_LCD_Init();
	    BSP_OLED_Init();

	    /* 5. Sensors & Actuators */
	    BSP_Relay_Init();
	    BSP_Sensors_Init();
	    BSP_LDR_init();
	    BSP_Delay_3s();
	    UART_Printf("System Boot Complete\r\n");
}

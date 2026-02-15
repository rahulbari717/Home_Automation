/*
 * bsp_init.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Board Support Package - System Initialization
 */


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
	    USART2_GPIOInit();
	    USART2_Init();
	    BSP_Delay_100ms();

	    /* ===== Print System Speeds ===== */
		uint32_t sysclk = RCC_GetSystemClock();
		uint32_t hclk   = RCC_GetHCLKFreq();
		uint32_t pclk1  = RCC_GetPCLK1Value();
		uint32_t pclk2  = RCC_GetPCLK2Value();

		UART_Printf("\r\n--- Clock Configuration Complete ---\r\n");
		UART_Printf("SYSCLK (Core): %u MHz\r\n", sysclk / 1000000);
		UART_Printf("HCLK   (AHB):  %u MHz\r\n", hclk / 1000000);
		UART_Printf("PCLK1  (APB1): %u MHz\r\n", pclk1 / 1000000);
		UART_Printf("PCLK2  (APB2): %u MHz\r\n", pclk2 / 1000000);
		UART_Printf("------------------------------------\r\n");

	    /* 3. User Interface */
	    BSP_LED_Init();
	    BSP_Buzzer_Init();
	    BSP_Delay_100ms();

	    // BSP_Button_Init();
		BSP_Button_Init();      // Initialize PC13 button with EXTI
	    BSP_Delay_100ms();
    	PC8_Button_Init();      // Initialize PC8 button with EXTI
	    BSP_Delay_100ms();
    	PC6_IR_Init();          // Initialize PC6 IR sensor with EXTI
	    BSP_Delay_100ms();
	    Keypad_Init();

	    /* 4. Displays */
	    BSP_LCD_Init();
	    BSP_OLED_Init();
	    BSP_Delay_100ms();
//		print_Log(" Home Automation ");
	    /* 5. Sensors & Actuators */
	    BSP_Relay_Init();
	    BSP_LDR_init();
	    BSP_Delay_1s();
	    // Success Tone
	    Device_PlayBuzzer(BEEP_SUCCESS);
	    UART_Printf("System Init Complete\r\n");

}

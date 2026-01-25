/*
 * 010_adc_led.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 * Description: Turns Onboard LED (PA5) ON when Dark (>3500)
 */

#include "bsp_init.h"
#include "bsp_uart2_debug.h"
#include "bsp_adc_sensors.h"

USART_Handle_t usart2_handle;

#define LDR_THRESHOLD 		(4000)

// Small helper to implement a blocking delay
void delay_ms(uint32_t count) {
    for(uint32_t i = 0; i < (count * 1000); i++);
}

int main(void)
{
    // 1. System Initialization
	app_init();

    // 2. UI Setup
    Debug_ClearScreen();
    UART_Printf("========================================\r\n");
    UART_Printf(">> [TEST] The Citadel: Sensor Monitor   \r\n");
    UART_Printf(">> [TEST] Dual LDR Monitor Started      \r\n");
    UART_Printf(">> LDR1: PA0 (CH0) | LDR2: PA1 (CH1)    \r\n");
    UART_Printf(">> [INFO] LDR: PA1 (ADC1_IN1) and relay driver \r\n");
    UART_Printf("========================================\r\n");

    uint16_t ldr1 = 0, ldr2 = 0;

	while(1)
	{
		// Read LDR 1 using the channel macro from config.h
		ldr1 = BSP_Sensor_ReadLDR(SENSOR_LDR1_CHANNEL);

		// Read LDR 2
		ldr2 = BSP_Sensor_ReadLDR(SENSOR_LDR2_CHANNEL);

		// Print values formatted nicely
		UART_Printf("LDR1 (Hall): %4d | LDR2 (Gate): %4d\r\n", ldr1, ldr2);

		// 4. Check Logic
		if(ldr1 > LDR_THRESHOLD)
		{
			// DARK: Turn LED ON (Set Pin 5)
//			GPIOA->ODR |= (1 << 5);
			GPIO_WriteToOutputPin(GPIOA, LED_GREEN_PIN, GPIO_PIN_SET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_RESET);
			UART_Printf("LDR: %4d  |  Status: DARK  (LED ON) \r\n", ldr1);
		}

		else
		{
			// LIGHT: Turn LED OFF (Clear Pin 5)
			GPIO_WriteToOutputPin(GPIOA, LED_GREEN_PIN, GPIO_PIN_RESET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_12, GPIO_PIN_SET);
			UART_Printf("LDR: %4d  |  Status: LIGHT (LED OFF)\r\n", ldr1);
		}


		if(ldr2 > LDR_THRESHOLD)
		{
			// DARK: Turn LED ON (Set Pin 5)
			// GPIOA->ODR |= (1 << 5);
			GPIO_WriteToOutputPin(GPIOA, LED_RED_PIN, GPIO_PIN_SET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_13, GPIO_PIN_RESET);
			UART_Printf("LDR: %4d  |  Status: DARK  (LED ON) \r\n", ldr2);
		}
		else
		{
			// LIGHT: Turn LED OFF (Clear Pin 5)
			GPIO_WriteToOutputPin(GPIOA, LED_RED_PIN, GPIO_PIN_RESET);
			GPIO_WriteToOutputPin(GPIOB, GPIO_PIN_NO_13, GPIO_PIN_SET);
			UART_Printf("LDR: %4d  |  Status: LIGHT (LED OFF)\r\n", ldr2);
		}

		// 5. Delay 500ms
		delay_ms(300);
	}
}

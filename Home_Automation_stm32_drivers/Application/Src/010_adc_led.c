/*
 * 010_adc_led.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 * Description: Turns Onboard LED (PA5) ON when Dark (>3500)
 */

#include "stm32f446xx.h"
#include "bsp_uart2_debug.h"
#include "bsp_adc_sensors.h"

#define LDR_THRESHOLD 		(3500)
// Small helper to implement a blocking delay
void delay_ms(uint32_t count) {
    for(uint32_t i = 0; i < (count * 1000); i++);
}

void gpio_init(){
	GPIO_Handle_t GpioLed;
	GpioLed.pGPIOx = GPIOA;
	GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
	GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
	GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

	GPIO_Init(&GpioLed);
}

int main(void)
{
    // 1. System Initialization
    Debug_Init();         // Initialize UART2
    gpio_init(); 		  // for led
    BSP_Sensors_Init();   // Initialize ADC & GPIOs for Pot/LDR

    // 2. UI Setup
    Debug_ClearScreen();
    UART_Printf("========================================\r\n");
    UART_Printf(">> [TEST] The Citadel: Sensor Monitor   \r\n");
    UART_Printf(">> [INFO] LDR: PA1 (ADC1_IN1) and led pa5          \r\n");
    UART_Printf("========================================\r\n");

    uint16_t ldr_val = 0;

	while(1)
	{
		// 3. Read LDR Value
		ldr_val = BSP_Sensor_ReadLDR();

		// 4. Print value
		// Dark  = High Resistance = Voltage changes (usually lower value)
		// Light = Low Resistance  = Voltage changes (usually higher value)
		UART_Printf("LDR Value: %4d\r\n", ldr_val);

		// 4. Check Logic
		if(ldr_val > LDR_THRESHOLD)
		{
			// DARK: Turn LED ON (Set Pin 5)
			GPIOA->ODR |= (1 << 5);
			UART_Printf("LDR: %4d  |  Status: DARK  (LED ON) \r\n", ldr_val);
		}
		else
		{
			// LIGHT: Turn LED OFF (Clear Pin 5)
			GPIOA->ODR &= ~(1 << 5);
			UART_Printf("LDR: %4d  |  Status: LIGHT (LED OFF)\r\n", ldr_val);
		}
		// 5. Delay 500ms
		delay_ms(1000);
	}
}

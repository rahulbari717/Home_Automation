/*
 * main.h
 *
 *  Created on: Jan 19, 2026
 *      Author: Rahul B.
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

/* 1. Standard C Libraries (Optional but useful) */
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* 2. The Silicon (Hardware Definitions) */
#include "stm32f446xx.h"
#include "config.h"

/* 3. The Drivers (Hardware Abstraction) */
#include "stm32f446xx_adc_driver.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_spi_driver.h"
#include "stm32f446xx_i2c_driver.h"
#include "stm32f446xx_usart_driver.h"
#include "stm32f446xx_rcc_driver.h"
#include "stm32f446xx_rtc_driver.h"
#include "stm32f446xx_timer_driver.h"
#include "stm32f446xx_iwdg_driver.h"
#include "stm32f446xx_fault_handler.h"

/* 4. (BSP Abstraction) */
#include "bsp_init.h"
#include "bsp_led.h"
#include "bsp_buzzer.h"
#include "bsp_relay.h"
#include "bsp_sensors.h"
#include "bsp_ldr.h"
#include "bsp_ds18b20.h"
#include "bsp_i2c_oled.h"
#include "bsp_lcd.h"
#include "bsp_bluetooth.h"
#include "bsp_keypad.h"
#include "bsp_delay.h"
#include "bsp_uart2_debug.h"
#include "bsp_button.h"

#endif /* INC_MAIN_H_ */

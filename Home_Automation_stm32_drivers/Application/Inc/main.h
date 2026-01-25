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

#endif /* INC_MAIN_H_ */

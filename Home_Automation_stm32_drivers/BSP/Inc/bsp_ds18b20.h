/*
 * bsp_ds18b20.h
 *
 *  Created on: Jan 26, 2026
 *      Author: lenovo
 */

#ifndef INC_BSP_DS18B20_H_
#define INC_BSP_DS18B20_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h"

void BSP_DS18B20_Init(void);
float BSP_DS18B20_ReadTemp(void);

#endif /* INC_BSP_DS18B20_H_ */

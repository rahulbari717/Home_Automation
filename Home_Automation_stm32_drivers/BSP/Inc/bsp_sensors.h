/*
 * bsp_sensors.h
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Motion & Proximity Sensors (PIR, IR, RCWL, HC-SR04)
 */

#ifndef INC_BSP_SENSORS_H_
#define INC_BSP_SENSORS_H_

#include "stm32f446xx_gpio_driver.h"
#include "config.h"

void BSP_Sensors_Init(void);
uint8_t BSP_Sensor_Read_Motion(void); // RCWL/PIR
uint8_t BSP_Sensor_Read_IR1(void);
uint8_t BSP_Sensor_Read_IR2(void);
uint8_t BSP_Sensor_Read_KY028(void);

#endif /* INC_BSP_SENSORS_H_ */

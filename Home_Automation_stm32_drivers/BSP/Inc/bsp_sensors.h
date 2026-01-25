/*
 * bsp_sensors.h
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 * Description: Motion & Proximity Sensors (PIR, IR, RCWL, HC-SR04)
 */

#ifndef INC_BSP_SENSORS_H_
#define INC_BSP_SENSORS_H_

#include "stm32f446xx.h"

/* Initialize all motion/proximity sensors */
void BSP_Sensors_Motion_Init(void);

/* Read RCWL Microwave Radar (1=motion, 0=no motion) */
uint8_t BSP_Sensor_Read_RCWL(void);

/* Read PIR sensors (1=motion, 0=no motion) */
uint8_t BSP_Sensor_Read_PIR1(void);
uint8_t BSP_Sensor_Read_PIR2(void);

/* Read IR proximity sensors (1=object detected, 0=clear) */
uint8_t BSP_Sensor_Read_IR1(void);
uint8_t BSP_Sensor_Read_IR2(void);

/* Read HC-SR04 distance in cm (2-400cm range) */
uint16_t BSP_Sensor_Read_HCSR04(void);

#endif /* INC_BSP_SENSORS_H_ */
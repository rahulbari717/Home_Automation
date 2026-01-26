/*
 * bsp_sensors.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 */

#include "bsp_sensors.h"

void BSP_Sensors_Init(void) {
    GPIO_Handle_t sensor;
    sensor.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    sensor.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    // RCWL and KY028 on Port B
    sensor.pGPIOx = GPIOB;
    sensor.GPIO_PinConfig.GPIO_PinNumber = RCWL_PIN;
    GPIO_Init(&sensor);
    sensor.GPIO_PinConfig.GPIO_PinNumber = KY028_DIGITAL_PIN;
    GPIO_Init(&sensor);

    // IR Sensors on Port C
    sensor.pGPIOx = GPIOC;
    sensor.GPIO_PinConfig.GPIO_PinNumber = IR1_PIN;
    GPIO_Init(&sensor);
    sensor.GPIO_PinConfig.GPIO_PinNumber = IR2_PIN;
    GPIO_Init(&sensor);
}

uint8_t BSP_Sensor_Read_Motion(void) { return GPIO_ReadFromInputPin(RCWL_PORT, RCWL_PIN); }
uint8_t BSP_Sensor_Read_IR1(void) 	 { return GPIO_ReadFromInputPin(IR1_PORT, IR1_PIN); }
uint8_t BSP_Sensor_Read_IR2(void) 	 { return GPIO_ReadFromInputPin(IR2_PORT, IR2_PIN); }
uint8_t BSP_Sensor_Read_KY028(void)  { return GPIO_ReadFromInputPin(KY028_DIGITAL_PORT, KY028_DIGITAL_PIN); }

/*
 * bsp_sensors.c
 *
 * Created on: Jan 24, 2026
 * Author: Rahul B.
 */

#include "bsp_sensors.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_timer_driver.h"
#include "config.h"

void BSP_Sensors_Motion_Init(void)
{
    GPIO_Handle_t sensor_gpio;
    
    // Configure all sensors as INPUT with pull-down
    sensor_gpio.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
    sensor_gpio.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    sensor_gpio.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PD;
    
    // RCWL (PC6)
    sensor_gpio.pGPIOx = RCWL_PORT;
    sensor_gpio.GPIO_PinConfig.GPIO_PinNumber = RCWL_PIN;
    GPIO_Init(&sensor_gpio);
    
    // IR1 (PC9)
//    sensor_gpio.pGPIOx = IR1_PORT;
//    sensor_gpio.GPIO_PinConfig.GPIO_PinNumber = IR1_PIN;
//    GPIO_Init(&sensor_gpio);
//
//    // IR2 (PC10)
//    sensor_gpio.pGPIOx = IR2_PORT;
//    sensor_gpio.GPIO_PinConfig.GPIO_PinNumber = IR2_PIN;
//    GPIO_Init(&sensor_gpio);
    
}

uint8_t BSP_Sensor_Read_RCWL(void)
{
    return GPIO_ReadFromInputPin(RCWL_PORT, RCWL_PIN);
}
//
//uint8_t BSP_Sensor_Read_IR1(void)
//{
//    // IR sensors output LOW when object detected
//    return !GPIO_ReadFromInputPin(IR1_PORT, IR1_PIN);
//}
//
//uint8_t BSP_Sensor_Read_IR2(void)
//{
//    return !GPIO_ReadFromInputPin(IR2_PORT, IR2_PIN);
//}

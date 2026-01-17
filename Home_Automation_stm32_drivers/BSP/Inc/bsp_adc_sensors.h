/*
 * bsp_adc_sensors.h
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 * Description: BSP for Potentiometer and LDR on The Citadel
 */

#ifndef INC_BSP_ADC_SENSORS_H_
#define INC_BSP_ADC_SENSORS_H_

#include "stm32f446xx.h"

/*
 * Hardware Pin Definitions
 * POT is connected to PA0 (ADC1_IN0)
 * LDR is connected to PA1 (ADC1_IN1)
 */
#define SENSOR_GPIO_PORT    GPIOA
#define SENSOR_POT_PIN      0       // PA0
#define SENSOR_LDR_PIN      1       // PA1

/*
 * Function Prototypes
 */

// Initialize GPIOs and ADC peripheral
void BSP_Sensors_Init(void);

// Read raw value from Potentiometer (0-4095)
uint16_t BSP_Sensor_ReadPot(void);

// Read raw value from LDR (0-4095)
uint16_t BSP_Sensor_ReadLDR(void);

#endif /* INC_BSP_ADC_SENSORS_H_ */

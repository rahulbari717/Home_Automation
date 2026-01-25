/*
 * bsp_adc_sensors.h
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 * Description: BSP for Potentiometer and LDR on The Citadel
 */

#ifndef INC_BSP_ADC_SENSORS_H_
#define INC_BSP_ADC_SENSORS_H_
#include "main.h"
/*
 * Function Prototypes
 */

// Read raw value from Potentiometer (0-4095)
uint16_t BSP_Sensor_ReadPot(void);

// Read raw value from LDR (0-4095)
uint16_t BSP_Sensor_ReadLDR(uint8_t channel);

#endif /* INC_BSP_ADC_SENSORS_H_ */

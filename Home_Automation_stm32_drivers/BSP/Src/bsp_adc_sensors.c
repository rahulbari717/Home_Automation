/*
 * bsp_adc_sensors.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 */

#include "bsp_adc_sensors.h"
#include "config.h"
/**
 * @brief Logic to read a specific LDR channel
 */
uint16_t BSP_Sensor_ReadLDR(uint8_t channel)
{
    ADC_Handle_t tempHandle;
    tempHandle.pADCx = SENSOR_ADC;

    // 1. Configure the ADC Channel to be read
    // Passing the handle pointer as required by your driver
    ADC_ConfigChannel(&tempHandle, channel, 1, ADC_SAMPLETIME_84);

    // 2. Start the conversion
    ADC_StartConversion(SENSOR_ADC);

    // 3. Wait for conversion to finish (Polling)
    // FIX: Changed ADC_FLAG_EOC to ADC_SR_EOC to match your driver
    while(ADC_GetFlagStatus(SENSOR_ADC, ADC_SR_EOC) == RESET);

    // 4. Return the result from Data Register
    return ADC_ReadValue(SENSOR_ADC);
}

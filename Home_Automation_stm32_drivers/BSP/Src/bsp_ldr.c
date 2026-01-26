/*
 * bsp_adc_sensors.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 */

#include <bsp_ldr.h>
#include "config.h"


/* ===== LDR/ADC Initialization ===== */
void BSP_LDR_init(void) {
    GPIO_Handle_t ldr_pins;
    ADC_Handle_t citadel_adc;

    GPIOA_PCLK_EN();
    ADC1_PCLK_EN();

    memset(&ldr_pins, 0, sizeof(ldr_pins));
    ldr_pins.pGPIOx = SENSOR_GPIO_PORT;
    ldr_pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ANALOG;
    ldr_pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;

    ldr_pins.GPIO_PinConfig.GPIO_PinNumber = SENSOR_LDR1_PIN;
    GPIO_Init(&ldr_pins);

    ldr_pins.GPIO_PinConfig.GPIO_PinNumber = SENSOR_LDR2_PIN;
    GPIO_Init(&ldr_pins);

    memset(&citadel_adc, 0, sizeof(citadel_adc));
    citadel_adc.pADCx = SENSOR_ADC;
    citadel_adc.ADC_Config.ADC_Resolution = ADC_RES_12BIT;
    citadel_adc.ADC_Config.ADC_ContinuousMode = ADC_CONT_DISABLE;
    citadel_adc.ADC_Config.ADC_DataAlign = ADC_ALIGN_RIGHT;
    citadel_adc.ADC_Config.ADC_ScanMode = ADC_SCAN_DISABLE;
    citadel_adc.ADC_Config.ADC_Prescaler = ADC_PRESCALER_DIV4;

    ADC_Init(&citadel_adc);
}

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

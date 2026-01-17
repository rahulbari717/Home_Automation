/*
 * bsp_adc_sensors.c
 *
 * Created on: Jan 17, 2026
 * Author: Rahul Bari
 */

#include "bsp_adc_sensors.h"
#include "stm32f446xx_adc_driver.h"

// Static handle for internal use
static ADC_Handle_t hADC1;

/*
 * @brief Initialize GPIOs (Analog Mode) and ADC1 Peripheral
 */
void BSP_Sensors_Init(void)
{
    // 1. Enable GPIO Clock (GPIOA)
    // Assuming you have a GPIO clock enable macro or function:
    RCC->AHB1ENR |= (1 << 0);

    // 2. Configure PA0 and PA1 to Analog Mode (Mode 3: 11)
    // Clear bits
    SENSOR_GPIO_PORT->MODER &= ~(3 << (2 * SENSOR_POT_PIN));
    SENSOR_GPIO_PORT->MODER &= ~(3 << (2 * SENSOR_LDR_PIN));

    // Set bits to 11 (Analog)
    SENSOR_GPIO_PORT->MODER |= (3 << (2 * SENSOR_POT_PIN));
    SENSOR_GPIO_PORT->MODER |= (3 << (2 * SENSOR_LDR_PIN));

    // 3. Initialize ADC1
    hADC1.pADCx = ADC1;

    hADC1.ADC_Config.ADC_Resolution       = ADC_RES_12BIT;
    hADC1.ADC_Config.ADC_ScanMode         = ADC_SCAN_DISABLE;      // Single channel at a time
    hADC1.ADC_Config.ADC_ContinuousMode   = ADC_CONT_DISABLE;      // One read per trigger
    hADC1.ADC_Config.ADC_ExternalTrigEdge = ADC_EXTTRIG_DISABLE;   // SW Trigger
    hADC1.ADC_Config.ADC_DataAlign        = ADC_ALIGN_RIGHT;
    hADC1.ADC_Config.ADC_NbrOfConversion  = 1;
    hADC1.ADC_Config.ADC_Prescaler        = ADC_PRESCALER_DIV4;    // Safer clock speed

    ADC_Init(&hADC1);

    // 4. Enable ADC (Power On)
    ADC_Enable(ADC1);
}

/*
 * @brief Configures ADC for Potentiometer (PA0/CH0), samples, and returns value.
 */
uint16_t BSP_Sensor_ReadPot(void)
{
    // 1. Configure Channel 0 as the 1st conversion in sequence
    // Sampling time: 84 cycles (approx good for Pot)
    ADC_ConfigChannel(&hADC1, ADC_CHANNEL_0, 1, ADC_SAMPLETIME_84);

    // 2. Start Conversion
    ADC_StartConversion(ADC1);

    // 3. Wait for conversion to complete
    ADC_PollForConversion(ADC1);

    // 4. Return result
    return ADC_ReadValue(ADC1);
}

/*
 * @brief Configures ADC for LDR (PA1/CH1), samples, and returns value.
 */
uint16_t BSP_Sensor_ReadLDR(void)
{
    // 1. Configure Channel 1 as the 1st conversion in sequence
    // Sampling time: 84 cycles
    ADC_ConfigChannel(&hADC1, ADC_CHANNEL_1, 1, ADC_SAMPLETIME_84);

    // 2. Start Conversion
    ADC_StartConversion(ADC1);

    // 3. Wait for conversion to complete
    ADC_PollForConversion(ADC1);

    // 4. Return result
    return ADC_ReadValue(ADC1);
}

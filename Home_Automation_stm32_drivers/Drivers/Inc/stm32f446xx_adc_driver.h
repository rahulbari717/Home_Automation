/*
 * stm32f446xx_adc_driver.h
 *
 * Created on: Jan 16, 2026
 * Author: Rahul Bari.
 */

#ifndef INC_STM32F446XX_ADC_DRIVER_H_
#define INC_STM32F446XX_ADC_DRIVER_H_

#include "stm32f446xx.h"
#include <stdint.h>
/*
 * @ADC_RESOLUTION
 */
#define ADC_RES_12BIT           0
#define ADC_RES_10BIT           1
#define ADC_RES_8BIT            2
#define ADC_RES_6BIT            3

/*
 * @ADC_CHANNELS
 */
#define ADC_CHANNEL_0           0
#define ADC_CHANNEL_1           1
#define ADC_CHANNEL_2           2
#define ADC_CHANNEL_3           3
#define ADC_CHANNEL_4           4
#define ADC_CHANNEL_5           5
#define ADC_CHANNEL_6           6
#define ADC_CHANNEL_7           7
#define ADC_CHANNEL_8           8
#define ADC_CHANNEL_9           9
#define ADC_CHANNEL_10          10
#define ADC_CHANNEL_11          11
#define ADC_CHANNEL_12          12
#define ADC_CHANNEL_13          13
#define ADC_CHANNEL_14          14
#define ADC_CHANNEL_15          15
#define ADC_CHANNEL_16          16
#define ADC_CHANNEL_17          17
#define ADC_CHANNEL_18          18

/*
 * @ADC_SAMPLE_TIME
 */
#define ADC_SAMPLETIME_3        0
#define ADC_SAMPLETIME_15       1
#define ADC_SAMPLETIME_28       2
#define ADC_SAMPLETIME_56       3
#define ADC_SAMPLETIME_84       4
#define ADC_SAMPLETIME_112      5
#define ADC_SAMPLETIME_144      6
#define ADC_SAMPLETIME_480      7

/*
 * @ADC_DATA_ALIGN
 */
#define ADC_ALIGN_RIGHT         0
#define ADC_ALIGN_LEFT          1

/*
 * @ADC_SCAN_MODE
 */
#define ADC_SCAN_DISABLE        0
#define ADC_SCAN_ENABLE         1

/*
 * @ADC_CONTINUOUS_MODE
 */
#define ADC_CONT_DISABLE        0
#define ADC_CONT_ENABLE         1

/*
 * @ADC_EOC_SELECTION
 */
#define ADC_EOC_SEQ             0
#define ADC_EOC_SINGLE          1

/*
 * @ADC_EXTERNAL_TRIGGER
 */
#define ADC_EXTTRIG_DISABLE     0
#define ADC_EXTTRIG_RISING      1
#define ADC_EXTTRIG_FALLING     2
#define ADC_EXTTRIG_BOTH        3

/*
 * @ADC_PRESCALER
 */
#define ADC_PRESCALER_DIV2      0
#define ADC_PRESCALER_DIV4      1
#define ADC_PRESCALER_DIV6      2
#define ADC_PRESCALER_DIV8      3

/*
 * ADC Status Register (SR) bit definitions
 */
#define ADC_SR_AWD              (1 << 0)
#define ADC_SR_EOC              (1 << 1)
#define ADC_SR_JEOC             (1 << 2)
#define ADC_SR_JSTRT            (1 << 3)
#define ADC_SR_STRT             (1 << 4)
#define ADC_SR_OVR              (1 << 5)

/*
 * Configuration structure for ADC
 */
typedef struct
{
    uint8_t ADC_Resolution;         /* @ADC_RESOLUTION */
    uint8_t ADC_DataAlign;          /* @ADC_DATA_ALIGN */
    uint8_t ADC_ScanMode;           /* @ADC_SCAN_MODE */
    uint8_t ADC_ContinuousMode;     /* @ADC_CONTINUOUS_MODE */
    uint8_t ADC_ExternalTrigEdge;   /* @ADC_EXTERNAL_TRIGGER */
    uint8_t ADC_EOCSelection;       /* @ADC_EOC_SELECTION */
    uint8_t ADC_NbrOfConversion;    /* Number of conversions (1-16) */
    uint8_t ADC_Prescaler;          /* <--- NEW: REQUIRED for ADC_Init logic */
} ADC_Config_t;

/*
 * Handle structure for ADC
 */
typedef struct
{
    ADC_RegDef_t *pADCx;
    ADC_Config_t ADC_Config;
} ADC_Handle_t;

/******************************************************************************************
 * APIs supported by this driver
 ******************************************************************************************/

/* Peripheral Clock setup */
void ADC_PeriClockControl(ADC_RegDef_t *pADCx, uint8_t EnorDi);

/* Init and De-init */
void ADC_Init(ADC_Handle_t *pADCHandle);
void ADC_DeInit(ADC_RegDef_t *pADCx);

/* Channel Configuration */
void ADC_ConfigChannel(ADC_Handle_t *pADCHandle, uint8_t Channel, uint8_t Rank, uint8_t SamplingTime);

/* ADC Enable/Disable */
void ADC_Enable(ADC_RegDef_t *pADCx);
void ADC_Disable(ADC_RegDef_t *pADCx);

/* Start/Stop Conversion */
void ADC_StartConversion(ADC_RegDef_t *pADCx);
void ADC_StopConversion(ADC_RegDef_t *pADCx);

/* Data read */
uint16_t ADC_ReadValue(ADC_RegDef_t *pADCx);
uint8_t ADC_GetFlagStatus(ADC_RegDef_t *pADCx, uint8_t FlagName);

/* Polling Helper */
void ADC_PollForConversion(ADC_RegDef_t *pADCx); /* <--- NEW: Missing Prototype added */

/* IRQ Configuration and ISR handling */
void ADC_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void ADC_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void ADC_IRQHandling(ADC_Handle_t *pADCHandle);

/* Temperature sensor and VREFINT */
void ADC_TempSensorVrefintCmd(uint8_t EnorDi);

/* Calibration */
void ADC_StartCalibration(ADC_RegDef_t *pADCx);

#endif /* INC_STM32F446XX_ADC_DRIVER_H_ */

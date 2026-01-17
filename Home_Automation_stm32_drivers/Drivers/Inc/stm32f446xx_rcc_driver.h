/*
 * stm32f446xx_rcc_driver.h
 *
 *  Created on: Dec 4, 2025
 *      Author: Rahul B.
 */

#ifndef INC_STM32F446XX_RCC_DRIVER_H_
#define INC_STM32F446XX_RCC_DRIVER_H_

#include "stm32f446xx.h"

/*
 * Oscillator Type Definitions
 */
#define RCC_OSCILLATORTYPE_NONE     (0x00000000U)
#define RCC_OSCILLATORTYPE_HSE      (0x00000001U)
#define RCC_OSCILLATORTYPE_HSI      (0x00000002U)
#define RCC_OSCILLATORTYPE_LSE      (0x00000004U)
#define RCC_OSCILLATORTYPE_LSI      (0x00000008U)

/*
 * HSE State Definitions
 */
#define RCC_HSE_OFF                 (0x00000000U)
#define RCC_HSE_ON                  (0x00000001U)
#define RCC_HSE_BYPASS              (0x00000005U)

/*
 * HSI State Definitions
 */
#define RCC_HSI_OFF                 (0x00000000U)
#define RCC_HSI_ON                  (0x00000001U)

/*
 * LSE State Definitions
 */
#define RCC_LSE_OFF                 (0x00000000U)
#define RCC_LSE_ON                  (0x00000001U)
#define RCC_LSE_BYPASS              (0x00000005U)

/*
 * LSI State Definitions
 */
#define RCC_LSI_OFF                 (0x00000000U)
#define RCC_LSI_ON                  (0x00000001U)

/*
 * PLL State Definitions
 */
#define RCC_PLL_NONE                (0x00000000U)
#define RCC_PLL_OFF                 (0x00000001U)
#define RCC_PLL_ON                  (0x00000002U)

/*
 * PLL Clock Source
 */
#define RCC_PLLSOURCE_HSI           (0x00000000U)
#define RCC_PLLSOURCE_HSE           (0x00400000U)

/*
 * System Clock Type Definitions
 */
#define RCC_CLOCKTYPE_SYSCLK        (0x00000001U)
#define RCC_CLOCKTYPE_HCLK          (0x00000002U)
#define RCC_CLOCKTYPE_PCLK1         (0x00000004U)
#define RCC_CLOCKTYPE_PCLK2         (0x00000008U)

/*
 * System Clock Source
 */
#define RCC_SYSCLKSOURCE_HSI        (0x00000000U)
#define RCC_SYSCLKSOURCE_HSE        (0x00000001U)
#define RCC_SYSCLKSOURCE_PLLCLK     (0x00000002U)

/*
 * AHB Clock Divider
 */
#define RCC_SYSCLK_DIV1             (0x00000000U)
#define RCC_SYSCLK_DIV2             (0x00000080U)
#define RCC_SYSCLK_DIV4             (0x00000090U)
#define RCC_SYSCLK_DIV8             (0x000000A0U)
#define RCC_SYSCLK_DIV16            (0x000000B0U)
#define RCC_SYSCLK_DIV64            (0x000000C0U)
#define RCC_SYSCLK_DIV128           (0x000000D0U)
#define RCC_SYSCLK_DIV256           (0x000000E0U)
#define RCC_SYSCLK_DIV512           (0x000000F0U)

/*
 * APB1 Clock Divider
 */
#define RCC_HCLK_DIV1               (0x00000000U)
#define RCC_HCLK_DIV2               (0x00001000U)
#define RCC_HCLK_DIV4               (0x00001400U)
#define RCC_HCLK_DIV8               (0x00001800U)
#define RCC_HCLK_DIV16              (0x00001C00U)

/*
 * Flash Latency
 */
typedef enum {
    RCC_FLASH_LATENCY_0WS = 0,
    RCC_FLASH_LATENCY_1WS,
    RCC_FLASH_LATENCY_2WS,
    RCC_FLASH_LATENCY_3WS,
    RCC_FLASH_LATENCY_4WS,
    RCC_FLASH_LATENCY_5WS
} RCC_FlashLatency_t;

/*
 * PLL Configuration Structure
 */
typedef struct
{
    uint32_t PLLState;      /* PLL state: RCC_PLL_NONE, RCC_PLL_OFF, RCC_PLL_ON */
    uint32_t PLLSource;     /* PLL source: RCC_PLLSOURCE_HSI or RCC_PLLSOURCE_HSE */
    uint32_t PLLM;          /* Division factor for PLL input (2 to 63) */
    uint32_t PLLN;          /* Multiplication factor for VCO (50 to 432) */
    uint32_t PLLP;          /* Division factor for main system clock (2, 4, 6, or 8) */
    uint32_t PLLQ;          /* Division factor for USB OTG FS, SDIO (2 to 15) */
    uint32_t PLLR;          /* Division factor for I2S, SAI, etc (2 to 7) */
} RCC_PLLInitTypeDef;

/*
 * RCC Oscillator Configuration Structure
 */
typedef struct
{
    uint32_t OscillatorType;       /* Oscillator type: HSE, HSI, LSE, LSI */
    uint32_t HSEState;             /* HSE state: OFF, ON, BYPASS */
    uint32_t LSEState;             /* LSE state: OFF, ON, BYPASS */
    uint32_t HSIState;             /* HSI state: OFF, ON */
    uint32_t HSICalibrationValue;  /* HSI calibration trimming value (0 to 31) */
    uint32_t LSIState;             /* LSI state: OFF, ON */
    RCC_PLLInitTypeDef PLL;        /* PLL configuration */
} RCC_OscInitTypeDef;

/*
 * RCC Clock Configuration Structure
 */
typedef struct
{
    uint32_t ClockType;            /* Clock type: SYSCLK, HCLK, PCLK1, PCLK2 */
    uint32_t SYSCLKSource;         /* System clock source: HSI, HSE, PLL */
    uint32_t AHBCLKDivider;        /* AHB clock divider */
    uint32_t APB1CLKDivider;       /* APB1 clock divider */
    uint32_t APB2CLKDivider;       /* APB2 clock divider */
} RCC_ClkInitTypeDef;

/*
 * Status Return Values
 */
typedef enum
{
    RCC_OK      = 0x00U,
    RCC_ERROR   = 0x01U,
    RCC_TIMEOUT = 0x02U
} RCC_StatusTypeDef;

/*
 * API Function Prototypes
 */

/* Main Configuration APIs */
RCC_StatusTypeDef RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
RCC_StatusTypeDef RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);

/* Legacy Simple APIs */
void RCC_SetSystemClock_HSE(uint32_t targetFreq, uint32_t mode);
void RCC_SetSystemClock_HSI(void);
void SystemClock_Config_HSE_180MHz(void);

/* Get Clock Values */
uint32_t RCC_GetPCLK1Value(void);
uint32_t RCC_GetPCLK2Value(void);
uint32_t RCC_GetPLLOutputClock(void);
uint32_t RCC_GetSystemClock(void);
uint32_t RCC_GetHCLKFreq(void);

/* Helper Macros for Easy Configuration */
#define RCC_OSC_HSE_8MHZ_BYPASS() \
    { \
        .OscillatorType = RCC_OSCILLATORTYPE_HSE, \
        .HSEState = RCC_HSE_BYPASS, \
        .PLL = {.PLLState = RCC_PLL_NONE} \
    }

#define RCC_OSC_HSI_16MHZ() \
    { \
        .OscillatorType = RCC_OSCILLATORTYPE_HSI, \
        .HSIState = RCC_HSI_ON, \
        .HSICalibrationValue = 16, \
        .PLL = {.PLLState = RCC_PLL_NONE} \
    }

#define RCC_CLK_CONFIG_DEFAULT() \
    { \
        .ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2), \
        .AHBCLKDivider = RCC_SYSCLK_DIV1, \
        .APB1CLKDivider = RCC_HCLK_DIV1, \
        .APB2CLKDivider = RCC_HCLK_DIV1 \
    }

#endif /* INC_STM32F446XX_RCC_DRIVER_H_ */

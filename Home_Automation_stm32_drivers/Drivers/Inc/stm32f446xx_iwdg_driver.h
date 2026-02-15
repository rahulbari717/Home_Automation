/*
 * stm32f446xx_iwdg_driver.h
 *
 *  Created on: Jan 17, 2026
 *      Author: Rahul B.
 */

#ifndef INC_STM32F446XX_IWDG_DRIVER_H_
#define INC_STM32F446XX_IWDG_DRIVER_H_

#include "stm32f446xx.h"
#include <stdint.h>

/*
 * @IWDG_KEY_VALUES
 * IWDG Key Register Values
 */
#define IWDG_KEY_ENABLE         0xCCCC      /* Enable IWDG */
#define IWDG_KEY_RELOAD         0xAAAA      /* Reload counter */
#define IWDG_KEY_WRITE_ACCESS   0x5555      /* Enable write access to PR and RLR */

/*
 * @IWDG_PRESCALER
 * IWDG Prescaler Values
 * LSI Clock = 32kHz (typical)
 * Timeout = (Prescaler * Reload) / LSI_Freq
 */
#define IWDG_PRESCALER_4        0       /* Divider /4    -> 8 kHz   -> Max timeout: ~512ms  */
#define IWDG_PRESCALER_8        1       /* Divider /8    -> 4 kHz   -> Max timeout: ~1.024s */
#define IWDG_PRESCALER_16       2       /* Divider /16   -> 2 kHz   -> Max timeout: ~2.048s */
#define IWDG_PRESCALER_32       3       /* Divider /32   -> 1 kHz   -> Max timeout: ~4.096s */
#define IWDG_PRESCALER_64       4       /* Divider /64   -> 500 Hz  -> Max timeout: ~8.192s */
#define IWDG_PRESCALER_128      5       /* Divider /128  -> 250 Hz  -> Max timeout: ~16.384s */
#define IWDG_PRESCALER_256      6       /* Divider /256  -> 125 Hz  -> Max timeout: ~32.768s */

/*
 * @IWDG_RELOAD_VALUE
 * IWDG Reload Register Values
 */
#define IWDG_RELOAD_MAX         0xFFF   /* Maximum reload value (12-bit) */

/*
 * IWDG Status Register Bits
 */
#define IWDG_SR_PVU             (1 << 0)    /* Prescaler Value Update */
#define IWDG_SR_RVU             (1 << 1)    /* Reload Value Update */

/*
 * LSI Clock Frequency (Hz)
 * Typical value: 32kHz (can vary from 17kHz to 47kHz)
 */
#define LSI_FREQ_MIN            17000
#define LSI_FREQ_TYP            32000
#define LSI_FREQ_MAX            47000

/*
 * IWDG Configuration Structure
 */
typedef struct
{
    uint8_t IWDG_Prescaler;     /* Prescaler value @IWDG_PRESCALER */
    uint16_t IWDG_Reload;       /* Reload value (0-4095) */
} IWDG_Config_t;

/*
 * IWDG Handle Structure
 */
typedef struct
{
    IWDG_RegDef_t *pIWDGx;      /* Base address of IWDG peripheral */
    IWDG_Config_t IWDG_Config;  /* IWDG configuration */
} IWDG_Handle_t;

/*
 * IWDG Status Return Values
 */
typedef enum
{
    IWDG_OK      = 0x00U,
    IWDG_ERROR   = 0x01U,
    IWDG_TIMEOUT = 0x02U
} IWDG_StatusTypeDef;

/******************************************************************************************
 *                              APIs supported by this driver
 *         For more information about the APIs, check the function definitions
 ******************************************************************************************/

/*
 * IWDG Initialization and Configuration
 */
IWDG_StatusTypeDef IWDG_Init(IWDG_Handle_t *pIWDGHandle);
void IWDG_Start(void);
void IWDG_Reload(void);

/*
 * IWDG Timeout Calculation
 */
uint32_t IWDG_CalculateTimeout(uint8_t Prescaler, uint16_t Reload);
IWDG_StatusTypeDef IWDG_ConfigureTimeout(uint32_t TimeoutMs);

/*
 * IWDG Status Functions
 */
uint8_t IWDG_GetFlagStatus(uint16_t IWDG_FLAG);
IWDG_StatusTypeDef IWDG_WaitForUpdate(void);

/*
 * Quick Configuration Macros
 */
#define IWDG_START()            IWDG_Start()
#define IWDG_RELOAD()           IWDG_Reload()
#define IWDG_REFRESH()          IWDG_Reload()
#define IWDG_KICK()             IWDG_Reload()

/*
 * Preset Timeout Configurations
 * Usage: IWDG_Init_XXXms()
 */
IWDG_StatusTypeDef IWDG_Init_100ms(void);   /* 100ms timeout */
IWDG_StatusTypeDef IWDG_Init_500ms(void);   /* 500ms timeout */
IWDG_StatusTypeDef IWDG_Init_1s(void);      /* 1 second timeout */
IWDG_StatusTypeDef IWDG_Init_2s(void);      /* 2 seconds timeout */
IWDG_StatusTypeDef IWDG_Init_5s(void);      /* 5 seconds timeout */
IWDG_StatusTypeDef IWDG_Init_10s(void);     /* 10 seconds timeout */

#endif /* INC_STM32F446XX_IWDG_DRIVER_H_ */

/*
 * stm32f446xx_timer_driver.h
 *
 *  Created on: Jan 16, 2026
 *      Author: Rahul B.
 */

#ifndef INC_STM32F446XX_TIMER_DRIVER_H_
#define INC_STM32F446XX_TIMER_DRIVER_H_

#include "stm32f446xx.h"
#include <stdint.h>

/*
 * @TIMER_COUNTER_MODE
 * Timer counter mode
 */
#define TIMER_MODE_UP           0       /* Counter counts up */
#define TIMER_MODE_DOWN         1       /* Counter counts down */
#define TIMER_MODE_CENTER1      2       /* Center-aligned mode 1 */
#define TIMER_MODE_CENTER2      3       /* Center-aligned mode 2 */
#define TIMER_MODE_CENTER3      4       /* Center-aligned mode 3 */

/*
 * @TIMER_CLOCK_DIVISION
 * Timer clock division
 */
#define TIMER_CKDIV_1           0       /* tDTS = tCK_INT */
#define TIMER_CKDIV_2           1       /* tDTS = 2 * tCK_INT */
#define TIMER_CKDIV_4           2       /* tDTS = 4 * tCK_INT */

/*
 * @TIMER_AUTO_RELOAD_PRELOAD
 * Auto-reload preload
 */
#define TIMER_ARR_NOTBUFFERED   0       /* ARR register not buffered */
#define TIMER_ARR_BUFFERED      1       /* ARR register buffered */

/*
 * @TIMER_MASTER_MODE
 * Timer master mode selection
 */
#define TIMER_MASTER_RESET      0       /* Reset - UG bit used as trigger output */
#define TIMER_MASTER_ENABLE     1       /* Enable - Counter enable used as trigger output */
#define TIMER_MASTER_UPDATE     2       /* Update - Update event used as trigger output */
#define TIMER_MASTER_COMPARE    3       /* Compare Pulse */
#define TIMER_MASTER_OC1REF     4       /* OC1REF used as trigger output */
#define TIMER_MASTER_OC2REF     5       /* OC2REF used as trigger output */
#define TIMER_MASTER_OC3REF     6       /* OC3REF used as trigger output */
#define TIMER_MASTER_OC4REF     7       /* OC4REF used as trigger output */

/*
 * @TIMER_CHANNEL
 * Timer channel selection
 */
#define TIMER_CHANNEL_1         0
#define TIMER_CHANNEL_2         1
#define TIMER_CHANNEL_3         2
#define TIMER_CHANNEL_4         3

/*
 * @TIMER_OC_MODE
 * Output compare mode
 */
#define TIMER_OC_MODE_FROZEN    0       /* Frozen - no effect on output */
#define TIMER_OC_MODE_ACTIVE    1       /* Set channel to active on match */
#define TIMER_OC_MODE_INACTIVE  2       /* Set channel to inactive on match */
#define TIMER_OC_MODE_TOGGLE    3       /* Toggle output on match */
#define TIMER_OC_MODE_FORCE_LOW 4       /* Force output low */
#define TIMER_OC_MODE_FORCE_HIGH 5      /* Force output high */
#define TIMER_OC_MODE_PWM1      6       /* PWM mode 1 */
#define TIMER_OC_MODE_PWM2      7       /* PWM mode 2 */

/*
 * @TIMER_OC_POLARITY
 * Output compare polarity
 */
#define TIMER_OC_POL_HIGH       0       /* Active high */
#define TIMER_OC_POL_LOW        1       /* Active low */

/*
 * @TIMER_OC_PRELOAD
 * Output compare preload
 */
#define TIMER_OC_PRELOAD_DI     0       /* Preload disabled */
#define TIMER_OC_PRELOAD_EN     1       /* Preload enabled */

/*
 * @TIMER_IC_POLARITY
 * Input capture polarity
 */
#define TIMER_IC_POL_RISING     0       /* Rising edge */
#define TIMER_IC_POL_FALLING    1       /* Falling edge */
#define TIMER_IC_POL_BOTH       2       /* Both edges */

/*
 * @TIMER_IC_SELECTION
 * Input capture selection
 */
#define TIMER_IC_SEL_DIRECTTI   1       /* ICx mapped on TIx */
#define TIMER_IC_SEL_INDIRECTTI 2       /* ICx mapped on TIy */
#define TIMER_IC_SEL_TRC        3       /* ICx mapped on TRC */

/*
 * @TIMER_IC_PRESCALER
 * Input capture prescaler
 */
#define TIMER_IC_PSC_DIV1       0       /* No prescaler */
#define TIMER_IC_PSC_DIV2       1       /* Capture every 2 events */
#define TIMER_IC_PSC_DIV4       2       /* Capture every 4 events */
#define TIMER_IC_PSC_DIV8       3       /* Capture every 8 events */

/*
 * @TIMER_IC_FILTER
 * Input capture filter (0-15)
 */
#define TIMER_IC_FILTER_NONE    0       /* No filter */

/*
 * Timer Status Register (SR) bit definitions
 */
#define TIMER_SR_UIF            (1 << 0)    /* Update interrupt flag */
#define TIMER_SR_CC1IF          (1 << 1)    /* Capture/Compare 1 interrupt flag */
#define TIMER_SR_CC2IF          (1 << 2)    /* Capture/Compare 2 interrupt flag */
#define TIMER_SR_CC3IF          (1 << 3)    /* Capture/Compare 3 interrupt flag */
#define TIMER_SR_CC4IF          (1 << 4)    /* Capture/Compare 4 interrupt flag */
#define TIMER_SR_TIF            (1 << 6)    /* Trigger interrupt flag */

/*
 * Timer DMA/Interrupt Enable Register (DIER) bit definitions
 */
#define TIMER_DIER_UIE          (1 << 0)    /* Update interrupt enable */
#define TIMER_DIER_CC1IE        (1 << 1)    /* Capture/Compare 1 interrupt enable */
#define TIMER_DIER_CC2IE        (1 << 2)    /* Capture/Compare 2 interrupt enable */
#define TIMER_DIER_CC3IE        (1 << 3)    /* Capture/Compare 3 interrupt enable */
#define TIMER_DIER_CC4IE        (1 << 4)    /* Capture/Compare 4 interrupt enable */

/*
 * Events to report to the Application
 */
#define TIMER_EVENT_UPDATE  0
#define TIMER_EVENT_CC1     1
#define TIMER_EVENT_CC2     2
#define TIMER_EVENT_CC3     3
#define TIMER_EVENT_CC4     4

/*
 * Configuration structure for Timer base
 */
typedef struct
{
    uint16_t TIMER_Prescaler;           /* Prescaler value (0-65535) */
    uint16_t TIMER_CounterMode;         /* Counter mode @TIMER_COUNTER_MODE */
    uint32_t TIMER_Period;              /* Auto-reload value (0-65535 for 16-bit, 0-4294967295 for 32-bit) */
    uint16_t TIMER_ClockDivision;       /* Clock division @TIMER_CLOCK_DIVISION */
    uint8_t TIMER_RepetitionCounter;    /* Repetition counter (only for TIM1/8) */
    uint8_t TIMER_AutoReloadPreload;    /* Auto-reload preload @TIMER_AUTO_RELOAD_PRELOAD */
} TIMER_Base_Config_t;

/*
 * Configuration structure for Timer output compare
 */
typedef struct
{
    uint8_t TIMER_OCMode;               /* Output compare mode @TIMER_OC_MODE */
    uint32_t TIMER_Pulse;               /* Pulse value (compare value) */
    uint8_t TIMER_OCPolarity;           /* Output polarity @TIMER_OC_POLARITY */
    uint8_t TIMER_OCPreload;            /* Output compare preload @TIMER_OC_PRELOAD */
} TIMER_OC_Config_t;

/*
 * Configuration structure for Timer input capture
 */
typedef struct
{
    uint8_t TIMER_ICPolarity;           /* Input capture polarity @TIMER_IC_POLARITY */
    uint8_t TIMER_ICSelection;          /* Input capture selection @TIMER_IC_SELECTION */
    uint8_t TIMER_ICPrescaler;          /* Input capture prescaler @TIMER_IC_PRESCALER */
    uint8_t TIMER_ICFilter;             /* Input capture filter @TIMER_IC_FILTER */
} TIMER_IC_Config_t;

/*
 * Handle structure for Timer
 */
typedef struct
{
    TIM_RegDef_t *pTIMx;                /* Base address of Timer peripheral */
    TIMER_Base_Config_t TIMER_Config;  /* Timer base configuration */
} TIMER_Handle_t;

/******************************************************************************************
 *                              APIs supported by this driver
 *         For more information about the APIs, check the function definitions
 ******************************************************************************************/

/*
 * Peripheral Clock setup
 */
void TIMER_PeriClockControl(TIM_RegDef_t *pTIMx, uint8_t EnorDi);

/*
 * Base Timer Init and De-init
 */
void TIMER_DelayInit(void);
void TIMER_BaseInit(TIMER_Handle_t *pTIMERHandle);
void TIMER_DeInit(TIM_RegDef_t *pTIMx);

/*
 * Timer Control
 */
void TIMER_Enable(TIM_RegDef_t *pTIMx);
void TIMER_Disable(TIM_RegDef_t *pTIMx);

/*
 * Counter operations
 */
void TIMER_SetCounter(TIM_RegDef_t *pTIMx, uint32_t Counter);
uint32_t TIMER_GetCounter(TIM_RegDef_t *pTIMx);

/*
 * Output Compare Configuration
 */
void TIMER_OC_Config(TIM_RegDef_t *pTIMx, uint8_t Channel, TIMER_OC_Config_t *pOCConfig);
void TIMER_OC_Start(TIM_RegDef_t *pTIMx, uint8_t Channel);
void TIMER_OC_Stop(TIM_RegDef_t *pTIMx, uint8_t Channel);

/*
 * PWM Configuration
 */
void TIMER_PWM_Config(TIM_RegDef_t *pTIMx, uint8_t Channel, TIMER_OC_Config_t *pOCConfig);
void TIMER_PWM_Start(TIM_RegDef_t *pTIMx, uint8_t Channel);
void TIMER_PWM_Stop(TIM_RegDef_t *pTIMx, uint8_t Channel);
void TIMER_PWM_SetDutyCycle(TIM_RegDef_t *pTIMx, uint8_t Channel, uint32_t DutyCycle);

/*
 * Input Capture Configuration
 */
void TIMER_IC_Config(TIM_RegDef_t *pTIMx, uint8_t Channel, TIMER_IC_Config_t *pICConfig);
void TIMER_IC_Start(TIM_RegDef_t *pTIMx, uint8_t Channel);
void TIMER_IC_Stop(TIM_RegDef_t *pTIMx, uint8_t Channel);
uint32_t TIMER_IC_ReadValue(TIM_RegDef_t *pTIMx, uint8_t Channel);

/*
 * Interrupt and flag management
 */
void TIMER_ITConfig(TIM_RegDef_t *pTIMx, uint16_t TIMER_IT, uint8_t EnorDi);
uint8_t TIMER_GetFlagStatus(TIM_RegDef_t *pTIMx, uint16_t TIMER_FLAG);
void TIMER_ClearFlag(TIM_RegDef_t *pTIMx, uint16_t TIMER_FLAG);

/*
 * IRQ Configuration and ISR handling
 */
void TIMER_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void TIMER_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void TIMER_IRQHandling(TIMER_Handle_t *pTIMERHandle);

/*
 * Delay function
 */
void TIMER_DelayMs(TIM_RegDef_t *pTIMx, uint32_t DelayMs);
void TIMER_DelayUs(TIM_RegDef_t *pTIMx, uint32_t DelayUs);
void TIMER_Basic_DelayMs(TIM_RegDef_t *pTIMx, uint16_t DelayMs);
void TIMER_Basic_DelayMs_IT(TIM_RegDef_t *pTIMx, uint16_t DelayMs);
#endif /* INC_STM32F446XX_TIMER_DRIVER_H_ */

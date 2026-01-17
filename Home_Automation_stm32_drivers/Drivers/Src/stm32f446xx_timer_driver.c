/*
 * stm32f446xx_timer_driver.c
 *
 *  Created on: Jan 16, 2026
 *      Author: Rahul B.
 */

/*
 * stm32f446re timer info :
 * 2 Advance timer ==> tim1 & tim8
 * general purpose 32 bit ==>  tim2 & tim5
 * general purpose 16 bit ==>  tim3 & tim4
 * basic timer ==>  tim6 & tim7
 * single channel ==> 10, 11, 13, 14
 * Double channel ==> 9 ,12
 *
 * */

#include "stm32f446xx_timer_driver.h"
#include "stm32f446xx_rcc_driver.h"
extern volatile uint8_t g_TimerDelayComplete;

/*********************************************************************
 * @fn              - TIMER_DelayInit
 * @brief           - Smart Initialization of TIM2 for 1 microsecond ticks
 *********************************************************************/
void TIMER_DelayInit(void)
{
    TIMER_Handle_t TimDelay;
    uint32_t timer_clock_freq = 0;

    // ==========================================================
    // STEP 1: ASK THE SYSTEM "HOW FAST AM I RUNNING?"
    // ==========================================================

    // 1. Get the APB1 Bus Clock (PCLK1) using your RCC Driver
    uint32_t pclk1 = RCC_GetPCLK1Value();

    // 2. Determine Timer Input Frequency (Hardware Rule)
    // Rule: If APB1 Prescaler = 1, Timer Clock = PCLK1
    //       If APB1 Prescaler > 1, Timer Clock = 2 * PCLK1

    // Read APB1 Prescaler bits (Bits 12:10 of CFGR)
    // Note: Your RCC driver uses Shift 10 for PPRE1
    uint8_t apb1_prescaler = (RCC->CFGR >> 10) & 0x7;

    if(apb1_prescaler < 4)
    {
        // Prescaler is 1 (Not divided)
        timer_clock_freq = pclk1;
    }
    else
    {
        // Prescaler is 2, 4, 8, or 16
        timer_clock_freq = pclk1 * 2;
    }

    // ==========================================================
    // STEP 2: CALCULATE THE PERFECT PRESCALER
    // ==========================================================
    // We want the timer to tick exactly 1,000,000 times per second (1 MHz).
    // Formula: PSC = (Input_Freq / Target_Freq) - 1

    uint16_t required_psc = (timer_clock_freq / 1000000) - 1;

    // ==========================================================
    // STEP 3: CONFIGURE TIM2
    // ==========================================================
    TimDelay.pTIMx = TIM2;
    TimDelay.TIMER_Config.TIMER_Prescaler = required_psc; // <--- The Calculated Value
    TimDelay.TIMER_Config.TIMER_Period = 0xFFFFFFFF;      // Max Range
    TimDelay.TIMER_Config.TIMER_CounterMode = TIMER_MODE_UP;
    TimDelay.TIMER_Config.TIMER_ClockDivision = TIMER_CKDIV_1;
    TimDelay.TIMER_Config.TIMER_AutoReloadPreload = TIMER_ARR_BUFFERED;

    TIMER_BaseInit(&TimDelay);
    TIMER_Enable(TIM2);
}

/*********************************************************************
 * @fn              - TIMER_PeriClockControl
 *
 * @brief           - This function enables or disables peripheral clock for the given Timer
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - ENABLE or DISABLE macros
 *
 * @return          - none
 *
 * @Note            - TIM1/8/9/10/11 are on APB2, TIM2-7/12-14 are on APB1
 */
void TIMER_PeriClockControl(TIM_RegDef_t *pTIMx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pTIMx == TIM1)
        {
            TIM1_PCLK_EN();
        }
        else if(pTIMx == TIM2)
        {
            TIM2_PCLK_EN();
        }
        else if(pTIMx == TIM3)
        {
            TIM3_PCLK_EN();
        }
        else if(pTIMx == TIM4)
        {
            TIM4_PCLK_EN();
        }
        else if(pTIMx == TIM5)
        {
            TIM5_PCLK_EN();
        }
        else if(pTIMx == TIM6)
        {
            TIM6_PCLK_EN();
        }
        else if(pTIMx == TIM7)
        {
            TIM7_PCLK_EN();
        }
        else if(pTIMx == TIM8)
        {
            TIM8_PCLK_EN();
        }
        else if(pTIMx == TIM9)
        {
            TIM9_PCLK_EN();
        }
        else if(pTIMx == TIM10)
        {
            TIM10_PCLK_EN();
        }
        else if(pTIMx == TIM11)
        {
            TIM11_PCLK_EN();
        }
        else if(pTIMx == TIM12)
        {
            TIM12_PCLK_EN();
        }
        else if(pTIMx == TIM13)
        {
            TIM13_PCLK_EN();
        }
        else if(pTIMx == TIM14)
        {
            TIM14_PCLK_EN();
        }
    }
    else
    {
        if(pTIMx == TIM1)
        {
            TIM1_PCLK_DI();
        }
        else if(pTIMx == TIM2)
        {
            TIM2_PCLK_DI();
        }
        else if(pTIMx == TIM3)
        {
            TIM3_PCLK_DI();
        }
        else if(pTIMx == TIM4)
        {
            TIM4_PCLK_DI();
        }
        else if(pTIMx == TIM5)
        {
            TIM5_PCLK_DI();
        }
        else if(pTIMx == TIM6)
        {
            TIM6_PCLK_DI();
        }
        else if(pTIMx == TIM7)
        {
            TIM7_PCLK_DI();
        }
        else if(pTIMx == TIM8)
        {
            TIM8_PCLK_DI();
        }
        else if(pTIMx == TIM9)
        {
            TIM9_PCLK_DI();
        }
        else if(pTIMx == TIM10)
        {
            TIM10_PCLK_DI();
        }
        else if(pTIMx == TIM11)
        {
            TIM11_PCLK_DI();
        }
        else if(pTIMx == TIM12)
        {
            TIM12_PCLK_DI();
        }
        else if(pTIMx == TIM13)
        {
            TIM13_PCLK_DI();
        }
        else if(pTIMx == TIM14)
        {
            TIM14_PCLK_DI();
        }
    }
}

/*********************************************************************
 * @fn              - TIMER_BaseInit
 *
 * @brief           - This function initializes the timer base configuration
 *
 * @param[in]       - Handle structure containing base address and configuration
 *
 * @return          - none
 *
 * @Note            - Configure CR1, ARR, PSC registers
 */
void TIMER_BaseInit(TIMER_Handle_t *pTIMERHandle)
{
    uint32_t temp = 0;

    // Enable the peripheral clock
    TIMER_PeriClockControl(pTIMERHandle->pTIMx, ENABLE);

    // 1. Configure CR1 register
    temp = pTIMERHandle->pTIMx->CR1;

    // Configure counter mode (CMS and DIR bits)
    if(pTIMERHandle->TIMER_Config.TIMER_CounterMode == TIMER_MODE_UP)
    {
        temp &= ~(0x3 << 5);  // Clear CMS bits
        temp &= ~(1 << 4);    // Clear DIR bit (up counting)
    }
    else if(pTIMERHandle->TIMER_Config.TIMER_CounterMode == TIMER_MODE_DOWN)
    {
        temp &= ~(0x3 << 5);  // Clear CMS bits
        temp |= (1 << 4);     // Set DIR bit (down counting)
    }
    else
    {
        // Center-aligned modes
        temp &= ~(0x3 << 5);
        temp |= ((pTIMERHandle->TIMER_Config.TIMER_CounterMode - 1) << 5);
    }

    // Configure clock division (CKD bits)
    temp &= ~(0x3 << 8);
    temp |= (pTIMERHandle->TIMER_Config.TIMER_ClockDivision << 8);

    // Configure auto-reload preload (ARPE bit)
    if(pTIMERHandle->TIMER_Config.TIMER_AutoReloadPreload == TIMER_ARR_BUFFERED)
    {
        temp |= (1 << 7);
    }
    else
    {
        temp &= ~(1 << 7);
    }

    pTIMERHandle->pTIMx->CR1 = temp;

    // 2. Set prescaler value
    pTIMERHandle->pTIMx->PSC = pTIMERHandle->TIMER_Config.TIMER_Prescaler;

    // 3. Set auto-reload value
    pTIMERHandle->pTIMx->ARR = pTIMERHandle->TIMER_Config.TIMER_Period;

    // 4. Set repetition counter (only for TIM1 and TIM8)
    if((pTIMERHandle->pTIMx == TIM1) || (pTIMERHandle->pTIMx == TIM8))
    {
        pTIMERHandle->pTIMx->RCR = pTIMERHandle->TIMER_Config.TIMER_RepetitionCounter;
    }

    // 5. Generate update event to reload prescaler and ARR
    pTIMERHandle->pTIMx->EGR |= (1 << 0);  // UG bit
}

/*********************************************************************
 * @fn              - TIMER_DeInit
 *
 * @brief           - This function resets the given Timer peripheral
 *
 * @param[in]       - Base address of the Timer peripheral
 *
 * @return          - none
 *
 * @Note            - Uses RCC reset registers
 */
void TIMER_DeInit(TIM_RegDef_t *pTIMx)
{
    if(pTIMx == TIM1)
    {
        TIM1_REG_RESET();
    }
    else if(pTIMx == TIM2)
    {
        TIM2_REG_RESET();
    }
    else if(pTIMx == TIM3)
    {
        TIM3_REG_RESET();
    }
    else if(pTIMx == TIM4)
    {
        TIM4_REG_RESET();
    }
    else if(pTIMx == TIM5)
    {
        TIM5_REG_RESET();
    }
    else if(pTIMx == TIM6)
    {
        TIM6_REG_RESET();
    }
    else if(pTIMx == TIM7)
    {
        TIM7_REG_RESET();
    }
    else if(pTIMx == TIM8)
    {
        TIM8_REG_RESET();
    }
    else if(pTIMx == TIM9)
    {
        TIM9_REG_RESET();
    }
    else if(pTIMx == TIM10)
    {
        TIM10_REG_RESET();
    }
    else if(pTIMx == TIM11)
    {
        TIM11_REG_RESET();
    }
    else if(pTIMx == TIM12)
    {
        TIM12_REG_RESET();
    }
    else if(pTIMx == TIM13)
    {
        TIM13_REG_RESET();
    }
    else if(pTIMx == TIM14)
    {
        TIM14_REG_RESET();
    }
}

/*********************************************************************
 * @fn              - TIMER_Enable
 *
 * @brief           - This function enables the timer counter
 *
 * @param[in]       - Base address of the Timer peripheral
 *
 * @return          - none
 *
 * @Note            - Sets CEN bit in CR1 register
 */
void TIMER_Enable(TIM_RegDef_t *pTIMx)
{
    pTIMx->CR1 |= (1 << 0);  // Set CEN bit
}

/*********************************************************************
 * @fn              - TIMER_Disable
 *
 * @brief           - This function disables the timer counter
 *
 * @param[in]       - Base address of the Timer peripheral
 *
 * @return          - none
 *
 * @Note            - Clears CEN bit in CR1 register
 */
void TIMER_Disable(TIM_RegDef_t *pTIMx)
{
    pTIMx->CR1 &= ~(1 << 0);  // Clear CEN bit
}

/*********************************************************************
 * @fn              - TIMER_SetCounter
 *
 * @brief           - This function sets the counter value
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Counter value
 *
 * @return          - none
 *
 * @Note            - Writes to CNT register
 */
void TIMER_SetCounter(TIM_RegDef_t *pTIMx, uint32_t Counter)
{
    pTIMx->CNT = Counter;
}

/*********************************************************************
 * @fn              - TIMER_GetCounter
 *
 * @brief           - This function reads the current counter value
 *
 * @param[in]       - Base address of the Timer peripheral
 *
 * @return          - Current counter value
 *
 * @Note            - Reads from CNT register
 */
uint32_t TIMER_GetCounter(TIM_RegDef_t *pTIMx)
{
    return pTIMx->CNT;
}

/*********************************************************************
 * @fn              - TIMER_OC_Config
 *
 * @brief           - This function configures output compare channel
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 * @param[in]       - Output compare configuration structure
 *
 * @return          - none
 *
 * @Note            - Configure CCMRx and CCER registers
 */
void TIMER_OC_Config(TIM_RegDef_t *pTIMx, uint8_t Channel, TIMER_OC_Config_t *pOCConfig)
{
    uint32_t temp = 0;

    // Disable the channel before configuration
    pTIMx->CCER &= ~(1 << (Channel * 4));

    // Configure output compare mode in CCMRx register
    if(Channel == TIMER_CHANNEL_1 || Channel == TIMER_CHANNEL_2)
    {
        temp = pTIMx->CCMR1;
        if(Channel == TIMER_CHANNEL_1)
        {
            temp &= ~(0x7 << 4);  // Clear OC1M bits
            temp |= (pOCConfig->TIMER_OCMode << 4);
            temp &= ~(1 << 3);    // Clear OC1PE bit
            temp |= (pOCConfig->TIMER_OCPreload << 3);
        }
        else
        {
            temp &= ~(0x7 << 12);  // Clear OC2M bits
            temp |= (pOCConfig->TIMER_OCMode << 12);
            temp &= ~(1 << 11);    // Clear OC2PE bit
            temp |= (pOCConfig->TIMER_OCPreload << 11);
        }
        pTIMx->CCMR1 = temp;
    }
    else
    {
        temp = pTIMx->CCMR2;
        if(Channel == TIMER_CHANNEL_3)
        {
            temp &= ~(0x7 << 4);  // Clear OC3M bits
            temp |= (pOCConfig->TIMER_OCMode << 4);
            temp &= ~(1 << 3);    // Clear OC3PE bit
            temp |= (pOCConfig->TIMER_OCPreload << 3);
        }
        else
        {
            temp &= ~(0x7 << 12);  // Clear OC4M bits
            temp |= (pOCConfig->TIMER_OCMode << 12);
            temp &= ~(1 << 11);    // Clear OC4PE bit
            temp |= (pOCConfig->TIMER_OCPreload << 11);
        }
        pTIMx->CCMR2 = temp;
    }

    // Set pulse value in CCRx register
    if(Channel == TIMER_CHANNEL_1)
        pTIMx->CCR1 = pOCConfig->TIMER_Pulse;
    else if(Channel == TIMER_CHANNEL_2)
        pTIMx->CCR2 = pOCConfig->TIMER_Pulse;
    else if(Channel == TIMER_CHANNEL_3)
        pTIMx->CCR3 = pOCConfig->TIMER_Pulse;
    else if(Channel == TIMER_CHANNEL_4)
        pTIMx->CCR4 = pOCConfig->TIMER_Pulse;

    // Configure polarity in CCER register
    if(pOCConfig->TIMER_OCPolarity == TIMER_OC_POL_LOW)
    {
        pTIMx->CCER |= (1 << ((Channel * 4) + 1));
    }
    else
    {
        pTIMx->CCER &= ~(1 << ((Channel * 4) + 1));
    }
}

/*********************************************************************
 * @fn              - TIMER_OC_Start
 *
 * @brief           - This function starts output compare on a channel
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - none
 *
 * @Note            - Enables the channel in CCER register
 */
void TIMER_OC_Start(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    // Enable the capture/compare channel
    pTIMx->CCER |= (1 << (Channel * 4));

    // Enable main output (for TIM1 and TIM8)
    if((pTIMx == TIM1) || (pTIMx == TIM8))
    {
        pTIMx->BDTR |= (1 << 15);  // MOE bit
    }

    // Enable the counter
    TIMER_Enable(pTIMx);
}

/*********************************************************************
 * @fn              - TIMER_OC_Stop
 *
 * @brief           - This function stops output compare on a channel
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - none
 *
 * @Note            - Disables the channel in CCER register
 */
void TIMER_OC_Stop(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    // Disable the capture/compare channel
    pTIMx->CCER &= ~(1 << (Channel * 4));
}

/*********************************************************************
 * @fn              - TIMER_PWM_Config
 *
 * @brief           - This function configures PWM mode on a channel
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 * @param[in]       - Output compare configuration (mode should be PWM1 or PWM2)
 *
 * @return          - none
 *
 * @Note            - Uses TIMER_OC_Config internally
 */
void TIMER_PWM_Config(TIM_RegDef_t *pTIMx, uint8_t Channel, TIMER_OC_Config_t *pOCConfig)
{
    TIMER_OC_Config(pTIMx, Channel, pOCConfig);
}

/*********************************************************************
 * @fn              - TIMER_PWM_Start
 *
 * @brief           - This function starts PWM generation
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - none
 *
 * @Note            - Uses TIMER_OC_Start internally
 */
void TIMER_PWM_Start(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    TIMER_OC_Start(pTIMx, Channel);
}

/*********************************************************************
 * @fn              - TIMER_PWM_Stop
 *
 * @brief           - This function stops PWM generation
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - none
 *
 * @Note            - Uses TIMER_OC_Stop internally
 */
void TIMER_PWM_Stop(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    TIMER_OC_Stop(pTIMx, Channel);
}

/*********************************************************************
 * @fn              - TIMER_PWM_SetDutyCycle
 *
 * @brief           - This function sets the PWM duty cycle
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 * @param[in]       - Duty cycle value (CCR value)
 *
 * @return          - none
 *
 * @Note            - DutyCycle should be between 0 and ARR value
 */
void TIMER_PWM_SetDutyCycle(TIM_RegDef_t *pTIMx, uint8_t Channel, uint32_t DutyCycle)
{
    if(Channel == TIMER_CHANNEL_1)
        pTIMx->CCR1 = DutyCycle;
    else if(Channel == TIMER_CHANNEL_2)
        pTIMx->CCR2 = DutyCycle;
    else if(Channel == TIMER_CHANNEL_3)
        pTIMx->CCR3 = DutyCycle;
    else if(Channel == TIMER_CHANNEL_4)
        pTIMx->CCR4 = DutyCycle;
}

/*********************************************************************
 * @fn              - TIMER_IC_Config
 *
 * @brief           - This function configures input capture channel
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 * @param[in]       - Input capture configuration structure
 *
 * @return          - none
 *
 * @Note            - Configure CCMRx and CCER registers
 */
void TIMER_IC_Config(TIM_RegDef_t *pTIMx, uint8_t Channel, TIMER_IC_Config_t *pICConfig)
{
    uint32_t temp = 0;

    // Disable the channel before configuration
    pTIMx->CCER &= ~(1 << (Channel * 4));

    // Configure input capture in CCMRx register
    if(Channel == TIMER_CHANNEL_1 || Channel == TIMER_CHANNEL_2)
    {
        temp = pTIMx->CCMR1;
        if(Channel == TIMER_CHANNEL_1)
        {
            temp &= ~(0x3 << 0);  // Clear CC1S bits
            temp |= (pICConfig->TIMER_ICSelection << 0);
            temp &= ~(0x3 << 2);  // Clear IC1PSC bits
            temp |= (pICConfig->TIMER_ICPrescaler << 2);
            temp &= ~(0xF << 4);  // Clear IC1F bits
            temp |= (pICConfig->TIMER_ICFilter << 4);
        }
        else
        {
            temp &= ~(0x3 << 8);  // Clear CC2S bits
            temp |= (pICConfig->TIMER_ICSelection << 8);
            temp &= ~(0x3 << 10); // Clear IC2PSC bits
            temp |= (pICConfig->TIMER_ICPrescaler << 10);
            temp &= ~(0xF << 12); // Clear IC2F bits
            temp |= (pICConfig->TIMER_ICFilter << 12);
        }
        pTIMx->CCMR1 = temp;
    }
    else
    {
        temp = pTIMx->CCMR2;
        if(Channel == TIMER_CHANNEL_3)
        {
            temp &= ~(0x3 << 0);  // Clear CC3S bits
            temp |= (pICConfig->TIMER_ICSelection << 0);
            temp &= ~(0x3 << 2);  // Clear IC3PSC bits
            temp |= (pICConfig->TIMER_ICPrescaler << 2);
            temp &= ~(0xF << 4);  // Clear IC3F bits
            temp |= (pICConfig->TIMER_ICFilter << 4);
        }
        else
        {
            temp &= ~(0x3 << 8);  // Clear CC4S bits
            temp |= (pICConfig->TIMER_ICSelection << 8);
            temp &= ~(0x3 << 10); // Clear IC4PSC bits
            temp |= (pICConfig->TIMER_ICPrescaler << 10);
            temp &= ~(0xF << 12); // Clear IC4F bits
            temp |= (pICConfig->TIMER_ICFilter << 12);
        }
        pTIMx->CCMR2 = temp;
    }

    // Configure polarity in CCER register
    temp = pTIMx->CCER;
    if(pICConfig->TIMER_ICPolarity == TIMER_IC_POL_FALLING)
    {
        temp |= (1 << ((Channel * 4) + 1));
        temp &= ~(1 << ((Channel * 4) + 3));
    }
    else if(pICConfig->TIMER_ICPolarity == TIMER_IC_POL_BOTH)
    {
        temp |= (1 << ((Channel * 4) + 1));
        temp |= (1 << ((Channel * 4) + 3));
    }
    else  // Rising edge
    {
        temp &= ~(1 << ((Channel * 4) + 1));
        temp &= ~(1 << ((Channel * 4) + 3));
    }
    pTIMx->CCER = temp;
}

/*********************************************************************
 * @fn              - TIMER_IC_Start
 *
 * @brief           - This function starts input capture
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - none
 *
 * @Note            - Enables the channel in CCER register
 */
void TIMER_IC_Start(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    // Enable the capture/compare channel
    pTIMx->CCER |= (1 << (Channel * 4));

    // Enable the counter
    TIMER_Enable(pTIMx);
}

/*********************************************************************
 * @fn              - TIMER_IC_Stop
 *
 * @brief           - This function stops input capture
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - none
 *
 * @Note            - Disables the channel in CCER register
 */
void TIMER_IC_Stop(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    // Disable the capture/compare channel
    pTIMx->CCER &= ~(1 << (Channel * 4));
}

/*********************************************************************
 * @fn              - TIMER_IC_ReadValue
 *
 * @brief           - This function reads the captured value
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Channel number (0-3)
 *
 * @return          - Captured value
 *
 * @Note            - Reads from CCRx register
 */
uint32_t TIMER_IC_ReadValue(TIM_RegDef_t *pTIMx, uint8_t Channel)
{
    uint32_t value = 0;

    if(Channel == TIMER_CHANNEL_1)
        value = pTIMx->CCR1;
    else if(Channel == TIMER_CHANNEL_2)
        value = pTIMx->CCR2;
    else if(Channel == TIMER_CHANNEL_3)
        value = pTIMx->CCR3;
    else if(Channel == TIMER_CHANNEL_4)
        value = pTIMx->CCR4;

    return value;
}

/*********************************************************************
 * @fn              - TIMER_ITConfig
 *
 * @brief           - This function enables or disables timer interrupts
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Interrupt flag (TIMER_DIER_UIE, etc.)
 * @param[in]       - ENABLE or DISABLE
 *
 * @return          - none
 *
 * @Note            - Configure DIER register
 */
void TIMER_ITConfig(TIM_RegDef_t *pTIMx, uint16_t TIMER_IT, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        pTIMx->DIER |= TIMER_IT;
    }
    else
    {
        pTIMx->DIER &= ~TIMER_IT;
    }
}

/*********************************************************************
 * @fn              - TIMER_GetFlagStatus
 *
 * @brief           - This function returns the status of a flag
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Flag name (TIMER_SR_UIF, etc.)
 *
 * @return          - FLAG_SET or FLAG_RESET
 *
 * @Note            - Checks SR register
 */
uint8_t TIMER_GetFlagStatus(TIM_RegDef_t *pTIMx, uint16_t TIMER_FLAG)
{
    if(pTIMx->SR & TIMER_FLAG)
    {
        return FLAG_SET;
    }
    return FLAG_RESET;
}

/*********************************************************************
 * @fn              - TIMER_ClearFlag
 *
 * @brief           - This function clears a timer flag
 *
 * @param[in]       - Base address of the Timer peripheral
 * @param[in]       - Flag name (TIMER_SR_UIF, etc.)
 *
 * @return          - none
 *
 * @Note            - Clears bit in SR register
 */
void TIMER_ClearFlag(TIM_RegDef_t *pTIMx, uint16_t TIMER_FLAG)
{
    pTIMx->SR &= ~TIMER_FLAG;
}
/*********************************************************************
 * @fn              - TIMER_IRQInterruptConfig
 *
 * @brief           - Configures NVIC Interrupt for Timer
 *
 * @param[in]       - IRQNumber
 * @param[in]       - EnorDi (ENABLE/DISABLE)
 *
 * @return          - none
 */
void TIMER_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ISER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            *NVIC_ISER2 |= (1 << (IRQNumber % 64));
        }
    }
    else
    {
        if(IRQNumber <= 31)
        {
            *NVIC_ICER0 |= (1 << IRQNumber);
        }
        else if(IRQNumber > 31 && IRQNumber < 64)
        {
            *NVIC_ICER1 |= (1 << (IRQNumber % 32));
        }
        else if(IRQNumber >= 64 && IRQNumber < 96)
        {
            *NVIC_ICER2 |= (1 << (IRQNumber % 64));
        }
    }
}

/*********************************************************************
 * @fn              - TIMER_IRQPriorityConfig
 *
 * @brief           - Configures Priority for Timer Interrupt
 *
 * @param[in]       - IRQNumber
 * @param[in]       - IRQPriority
 *
 * @return          - none
 */
void TIMER_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;
    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    *(NVIC_PR_BASE_ADDR + iprx) &= ~(0xFF << shift_amount);
    *(NVIC_PR_BASE_ADDR + iprx) |=  (IRQPriority << shift_amount);
}

/*********************************************************************
 * @fn              - TIMER_DelayUs
 *
 * @brief           - Microsecond delay using timer
 *
 * @param[in]       - Base address of Timer peripheral (use TIM2)
 * @param[in]       - Delay in microseconds (1 to 4,294,967,295)
 *
 * @return          - none
 *
 * @Note            - Blocking delay
 *                  - Accuracy: ±1 µs
 *                  - Call TIMER_DelayInit() first!
 */
void TIMER_DelayUs(TIM_RegDef_t *pTIMx, uint32_t DelayUs)
{
    uint32_t start = pTIMx->CNT;

    // Wait for DelayUs microseconds to elapse
    while((pTIMx->CNT - start) < DelayUs)
    {
        // Busy wait (handles 32-bit counter overflow automatically)
    }
}

/*********************************************************************
 * @fn              - TIMER_DelayMs
 *
 * @brief           - Millisecond delay using timer
 *
 * @param[in]       - Base address of Timer peripheral (use TIM2)
 * @param[in]       - Delay in milliseconds (1 to 4,294,967)
 *
 * @return          - none
 *
 * @Note            - Blocking delay
 *                  - Accuracy: ±1 µs
 *                  - Max delay: ~4294 seconds (~71 minutes)
 */
void TIMER_DelayMs(TIM_RegDef_t *pTIMx, uint32_t DelayMs)
{
    // Convert ms to µs (1 ms = 1000 µs)
    TIMER_DelayUs(pTIMx, DelayMs * 1000);
}



/*********************************************************************
 * @fn              - TIMER_Basic_DelayMs
 * @brief           - Smart Delay for TIM6/TIM7.
 * Automatically calculates Prescaler based on current system clock.
 * @param[in]       - pTIMx: TIM6 or TIM7
 * @param[in]       - DelayMs: Delay in milliseconds
 *********************************************************************/
void TIMER_Basic_DelayMs(TIM_RegDef_t *pTIMx, uint16_t DelayMs)
{
	TIMER_PeriClockControl(pTIMx, ENABLE);
    // =================================================================
    // STEP 1: CALCULATE TIMER CLOCK FREQUENCY (DYNAMICALLY)
    // =================================================================
    uint32_t timer_clock_freq = 0;
    uint32_t pclk1 = RCC_GetPCLK1Value(); // Get the current APB1 Bus speed
    uint8_t apb1_prescaler = (RCC->CFGR >> 10) & 0x7; // Read PPRE1 bits

    // Hardware Rule: If APB1 Prescaler = 1, TimerClk = PCLK1. Else, TimerClk = 2 * PCLK1.
    if(apb1_prescaler < 4) {
        timer_clock_freq = pclk1;
    } else {
        timer_clock_freq = pclk1 * 2;
    }

    // =================================================================
    // STEP 2: CONFIGURE TIMER FOR 10 kHz (0.1ms resolution)
    // =================================================================
    // We want 1 tick = 0.1ms (10,000 Hz)
    // Formula: PSC = (TimerClk / TargetFreq) - 1
    uint16_t psc_value = (timer_clock_freq / 10000) - 1;

    // Reset Timer
    pTIMx->CR1 = 0;
    pTIMx->SR  = 0;

    // Set Calculated Prescaler
    pTIMx->PSC = psc_value;

    // =================================================================
    // STEP 3: SET DURATION
    // =================================================================
    // Since 1 tick = 0.1ms, we need DelayMs * 10 ticks
    uint32_t arr_value = (DelayMs * 10);

    // Safety: Clamp to 16-bit max (65535)
    if(arr_value > 0xFFFF) arr_value = 0xFFFF;

    pTIMx->ARR = (uint16_t)(arr_value - 1);

    // =================================================================
    // STEP 4: RUN TIMER
    // =================================================================
    // Generate Update Event to load new PSC/ARR values immediately
    pTIMx->EGR |= (1 << 0);
    pTIMx->SR  &= ~(1 << 0); // Clear the flag EGR just caused

    // Start Timer
    pTIMx->CR1 |= (1 << 0);

    // Wait for Alarm (Flag)
    while( ! (pTIMx->SR & (1 << 0)) );

    // Stop and Clear
    pTIMx->SR &= ~(1 << 0);
    pTIMx->CR1 &= ~(1 << 0);
}

/*********************************************************************
 * @fn              - TIMER_Basic_DelayMs_IT
 * @brief           - Interrupt-based Delay for TIM6/TIM7.
 * @Note            - Requires TIM6_DAC_IRQHandler to be implemented!
 *********************************************************************/
void TIMER_Basic_DelayMs_IT(TIM_RegDef_t *pTIMx, uint16_t DelayMs)
{
    // 1. Enable Peripheral Clock
    TIMER_PeriClockControl(pTIMx, ENABLE);

    // =================================================================
    // STEP 1: CALCULATE TIMER CLOCK FREQUENCY
    // =================================================================
    uint32_t timer_clock_freq = 0;
    uint32_t pclk1 = RCC_GetPCLK1Value();
    uint8_t apb1_prescaler = (RCC->CFGR >> 10) & 0x7;

    if(apb1_prescaler < 4) {
        timer_clock_freq = pclk1;
    } else {
        timer_clock_freq = pclk1 * 2;
    }

    // =================================================================
    // STEP 2: CONFIGURE TIMER
    // =================================================================
    // 10 kHz Frequency (0.1ms resolution)
    uint16_t psc_value = (timer_clock_freq / 10000) - 1;

    pTIMx->CR1 = 0; // Stop Timer
    pTIMx->SR  = 0; // Clear Flags
    pTIMx->PSC = psc_value;

    // =================================================================
    // STEP 3: SET DURATION
    // =================================================================
    uint32_t arr_value = (DelayMs * 10);
    if(arr_value > 0xFFFF) arr_value = 0xFFFF; // Safety Clamp

    pTIMx->ARR = (uint16_t)(arr_value - 1);

    // Force Update (EGR) to load settings
    pTIMx->EGR |= (1 << 0);
    pTIMx->SR  &= ~(1 << 0); // Clear flag generated by EGR

    // =================================================================
    // STEP 4: INTERRUPT SETUP (The New Part)
    // =================================================================

    // Reset our Global Flag
    g_TimerDelayComplete = 0;

    // Enable "Update Interrupt Enable" (UIE) bit in DIER register
    pTIMx->DIER |= (1 << 0);

    // Enable NVIC (TIM6 is IRQ 54, TIM7 is IRQ 55)
    if(pTIMx == TIM6) {
        TIMER_IRQInterruptConfig(TIM6_DAC_IRQn, ENABLE);
    } else if (pTIMx == TIM7) {
        TIMER_IRQInterruptConfig(TIM7_IRQn, ENABLE);
    }

    // =================================================================
    // STEP 5: RUN & WAIT
    // =================================================================

    // Start Timer
    pTIMx->CR1 |= (1 << 0);

    // Wait for the ISR to set the global flag to 1
    while(g_TimerDelayComplete == 0);

    // Stop Timer
    pTIMx->CR1 &= ~(1 << 0);

    // Disable Interrupts (Clean up)
    pTIMx->DIER &= ~(1 << 0);
}

/*********************************************************************
 * @fn      - TIMER_IRQHandling
 * @brief   - Handles Timer Interrupts (Clears Flag & Signals App)
 * @param   - pTIMERHandle: pointer to the timer handle structure
 *********************************************************************/
void TIMER_IRQHandling(TIMER_Handle_t *pTIMERHandle)
{
    // 1. Get the Status of the Update Interrupt Flag (UIF)
    uint8_t temp1 = pTIMERHandle->pTIMx->SR & (1 << 0);

    // 2. Get the Status of the Update Interrupt Enable bit (UIE)
    uint8_t temp2 = pTIMERHandle->pTIMx->DIER & (1 << 0);

    // 3. If both are set, the interrupt occurred due to Update Event
    if(temp1 && temp2)
    {
        // Clear the Update Interrupt Flag
        pTIMERHandle->pTIMx->SR &= ~(1 << 0);

        // Signal the Application
        // We use 'extern' here to find the variable defined in main.c
        extern volatile uint8_t g_TimerDelayComplete;
        g_TimerDelayComplete = 1;
    }
}

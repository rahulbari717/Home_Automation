/*
 * stm32f446xx_adc_driver.c
 *
 * Created on: Jan 16, 2026
 * Author: Rahul Bari.
 * Updated: Fixes for Prescaler, IRQ Safety, and Robustness.
 */

#include "stm32f446xx_adc_driver.h"

/*********************************************************************
 * @fn              - ADC_PeriClockControl
 * @brief           - Enables/Disables peripheral clock
 */
void ADC_PeriClockControl(ADC_RegDef_t *pADCx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pADCx == ADC1) ADC1_PCLK_EN();
        else if(pADCx == ADC2) ADC2_PCLK_EN();
        else if(pADCx == ADC3) ADC3_PCLK_EN();
    }
    else
    {
        if(pADCx == ADC1) ADC1_PCLK_DI();
        else if(pADCx == ADC2) ADC2_PCLK_DI();
        else if(pADCx == ADC3) ADC3_PCLK_DI();
    }
}

/*********************************************************************
 * @fn              - ADC_Init
 * @brief           - Initializes ADC with Prescaler, CR1, CR2, and SQR
 */
void ADC_Init(ADC_Handle_t *pADCHandle)
{
    uint32_t temp = 0;

    // Enable the peripheral clock
    ADC_PeriClockControl(pADCHandle->pADCx, ENABLE);

    // ---------------------------------------------------------
    // 1. Configure the ADC_CR1 register
    // ---------------------------------------------------------
    temp = pADCHandle->pADCx->CR1;

    // Configure resolution (bits 25:24)
    temp &= ~(0x3 << 24);
    temp |= (pADCHandle->ADC_Config.ADC_Resolution << 24);

    // Configure scan mode (bit 8)
    temp &= ~(1 << 8);
    temp |= (pADCHandle->ADC_Config.ADC_ScanMode << 8);

    pADCHandle->pADCx->CR1 = temp;

    // ---------------------------------------------------------
    // 2. Configure the ADC_CR2 register
    // ---------------------------------------------------------
    temp = pADCHandle->pADCx->CR2;

    // Configure data alignment (bit 11)
    temp &= ~(1 << 11);
    temp |= (pADCHandle->ADC_Config.ADC_DataAlign << 11);

    // Configure continuous mode (bit 1)
    temp &= ~(1 << 1);
    temp |= (pADCHandle->ADC_Config.ADC_ContinuousMode << 1);

    // Configure EOC selection (bit 10 of CR2)
    temp &= ~(1 << 10);
    temp |= (pADCHandle->ADC_Config.ADC_EOCSelection << 10);

    // Configure external trigger edge (bits 29:28)
    temp &= ~(0x3 << 28);
    temp |= (pADCHandle->ADC_Config.ADC_ExternalTrigEdge << 28);

    // [FIX 5] Reset EXTSEL (bits 27:24) to 0 if we are just doing SW Trigger
    // This ensures we aren't accidentally listening to Timer 1 CC1
    if (pADCHandle->ADC_Config.ADC_ExternalTrigEdge == ADC_EXTTRIG_DISABLE) {
        temp &= ~(0xF << 24);
    }

    pADCHandle->pADCx->CR2 = temp;

    // ---------------------------------------------------------
    // 3. Configure number of conversions (SQR1)
    // ---------------------------------------------------------
    if(pADCHandle->ADC_Config.ADC_NbrOfConversion > 0 &&
       pADCHandle->ADC_Config.ADC_NbrOfConversion <= 16)
    {
        temp = pADCHandle->pADCx->SQR1;
        temp &= ~(0xF << 20);
        temp |= ((pADCHandle->ADC_Config.ADC_NbrOfConversion - 1) << 20);
        pADCHandle->pADCx->SQR1 = temp;
    }

    // ---------------------------------------------------------
    // [FIX 1] Configure ADC Prescaler (ADC_COMMON->CCR)
    // ---------------------------------------------------------
    temp = ADC_COMMON->CCR;
    temp &= ~(0x3 << 16); // Clear bits 16 and 17
    temp |= (pADCHandle->ADC_Config.ADC_Prescaler << 16);
    ADC_COMMON->CCR = temp;
}

/*********************************************************************
 * @fn              - ADC_DeInit
 * @brief           - Resets ADC peripheral
 */
void ADC_DeInit(ADC_RegDef_t *pADCx)
{
    if(pADCx == ADC1) ADC1_REG_RESET();
    else if(pADCx == ADC2) ADC2_REG_RESET();
    else if(pADCx == ADC3) ADC3_REG_RESET();
}

/*********************************************************************
 * @fn              - ADC_ConfigChannel
 * @brief           - Configures Channel rank and sampling time
 */
void ADC_ConfigChannel(ADC_Handle_t *pADCHandle, uint8_t Channel, uint8_t Rank, uint8_t SamplingTime)
{
    uint32_t temp = 0;

    // 1. Configure sampling time
    if(Channel > 9)
    {
        temp = pADCHandle->pADCx->SMPR1;
        temp &= ~(0x7 << ((Channel - 10) * 3));
        temp |= (SamplingTime << ((Channel - 10) * 3));
        pADCHandle->pADCx->SMPR1 = temp;
    }
    else
    {
        temp = pADCHandle->pADCx->SMPR2;
        temp &= ~(0x7 << (Channel * 3));
        temp |= (SamplingTime << (Channel * 3));
        pADCHandle->pADCx->SMPR2 = temp;
    }

    // 2. Configure sequence rank
    if(Rank < 7)
    {
        temp = pADCHandle->pADCx->SQR3;
        temp &= ~(0x1F << ((Rank - 1) * 5));
        temp |= (Channel << ((Rank - 1) * 5));
        pADCHandle->pADCx->SQR3 = temp;
    }
    else if(Rank < 13)
    {
        temp = pADCHandle->pADCx->SQR2;
        temp &= ~(0x1F << ((Rank - 7) * 5));
        temp |= (Channel << ((Rank - 7) * 5));
        pADCHandle->pADCx->SQR2 = temp;
    }
    else if(Rank < 17)
    {
        temp = pADCHandle->pADCx->SQR1;
        temp &= ~(0x1F << ((Rank - 13) * 5));
        temp |= (Channel << ((Rank - 13) * 5));
        pADCHandle->pADCx->SQR1 = temp;
    }
}

/*********************************************************************
 * @fn              - ADC_Enable
 * @brief           - Sets ADON bit
 */
void ADC_Enable(ADC_RegDef_t *pADCx)
{
    pADCx->CR2 |= (1 << 0);  // Set ADON bit
    // Note: A stabilization time (tSTAB) is technically required here before conversion starts
}

/*********************************************************************
 * @fn              - ADC_Disable
 * @brief           - Clears ADON bit
 */
void ADC_Disable(ADC_RegDef_t *pADCx)
{
    pADCx->CR2 &= ~(1 << 0);  // Clear ADON bit
}

/*********************************************************************
 * @fn              - ADC_StartConversion
 * @brief           - Starts conversion with safety check
 */
void ADC_StartConversion(ADC_RegDef_t *pADCx)
{
    // [FIX 2] Ensure ADC is enabled before starting
    if( !(pADCx->CR2 & (1 << 0)) )
    {
        pADCx->CR2 |= (1 << 0); // Enable ADON

        // Simple delay loop for stabilization (approx few microseconds)
        // In a real OS/HAL, use a proper delay function
        for(volatile int i=0; i<1000; i++);
    }

    pADCx->CR2 |= (1 << 30);  // Set SWSTART bit
}

/*********************************************************************
 * @fn              - ADC_StopConversion
 * @brief           - Stops conversion
 */
void ADC_StopConversion(ADC_RegDef_t *pADCx)
{
    // [FIX 4] Note: Single conversion cannot be easily aborted on F4 just by clearing bits
    // We only clear continuous mode here.
    pADCx->CR2 &= ~(1 << 1);  // Clear CONT bit
}

/*********************************************************************
 * @fn              - ADC_ReadValue
 * @brief           - Reads DR register
 */
uint16_t ADC_ReadValue(ADC_RegDef_t *pADCx)
{
    return (uint16_t)(pADCx->DR & 0xFFFF);
}

/*********************************************************************
 * @fn              - ADC_GetFlagStatus
 * @brief           - Returns flag status
 */
uint8_t ADC_GetFlagStatus(ADC_RegDef_t *pADCx, uint8_t FlagName)
{
    if(pADCx->SR & FlagName) return FLAG_SET;
    return FLAG_RESET;
}

/*********************************************************************
 * @fn              - ADC_IRQInterruptConfig
 * @brief           - Configures NVIC Enable/Disable
 */
void ADC_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(IRQNumber <= 31) *NVIC_ISER0 |= (1 << IRQNumber);
        else if(IRQNumber < 64) *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        else if(IRQNumber < 96) *NVIC_ISER2 |= (1 << (IRQNumber % 64));
    }
    else
    {
        if(IRQNumber <= 31) *NVIC_ICER0 |= (1 << IRQNumber);
        else if(IRQNumber < 64) *NVIC_ICER1 |= (1 << (IRQNumber % 32));
        else if(IRQNumber < 96) *NVIC_ICER2 |= (1 << (IRQNumber % 64));
    }
}

/*********************************************************************
 * @fn              - ADC_IRQPriorityConfig
 * @brief           - Configures NVIC Priority
 */
void ADC_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;
    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);

    // [FIX 3] Clear the bits before setting them
    *(NVIC_PR_BASE_ADDR + iprx) &= ~(0xFF << shift_amount);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/*********************************************************************
 * @fn              - ADC_IRQHandling
 * @brief           - Handles Interrupts
 */
void ADC_IRQHandling(ADC_Handle_t *pADCHandle)
{
    // Check EOC
    if(pADCHandle->pADCx->SR & ADC_SR_EOC)
    {
        // Hook for callback or data read
    }

    // Check OVR
    if(pADCHandle->pADCx->SR & ADC_SR_OVR)
    {
        pADCHandle->pADCx->SR &= ~ADC_SR_OVR; // Clear OVR
    }
}

/*********************************************************************
 * @fn              - ADC_TempSensorVrefintCmd
 * @brief           - Enables internal temp sensor
 */
void ADC_TempSensorVrefintCmd(uint8_t EnorDi)
{
    if(EnorDi == ENABLE) ADC_COMMON->CCR |= (1 << 23);
    else ADC_COMMON->CCR &= ~(1 << 23);
}

/*********************************************************************
 * @fn              - ADC_StartCalibration
 * @brief           - Compatibility dummy for F4
 */
void ADC_StartCalibration(ADC_RegDef_t *pADCx)
{
    (void)pADCx; // Not needed for F4
}

/*********************************************************************
 * @fn              - ADC_PollForConversion
 * @brief           - Blocking wait for conversion (Helper)
 */
void ADC_PollForConversion(ADC_RegDef_t *pADCx)
{
    // Wait until EOC is set
    while(! (pADCx->SR & ADC_SR_EOC) );
}

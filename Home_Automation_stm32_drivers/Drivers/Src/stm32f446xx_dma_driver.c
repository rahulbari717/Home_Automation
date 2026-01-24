/*
 * stm32f446xx_dma_driver.c
 *
 *  Created on: Jan 18, 2026
 *      Author: Rahul B.
 */

#include "stm32f446xx_dma_driver.h"

/*********************************************************************
 * @fn              - DMA_PeriClockControl
 * @brief           - Enables or disables DMA peripheral clock
 * @param[in]       - pDMAx: DMA1 or DMA2
 * @param[in]       - EnorDi: ENABLE or DISABLE
 ********************************************************************/
void DMA_PeriClockControl(DMA_RegDef_t *pDMAx, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(pDMAx == DMA1)
            RCC->AHB1ENR |= (1 << 21);
        else if(pDMAx == DMA2)
            RCC->AHB1ENR |= (1 << 22);
    }
    else
    {
        if(pDMAx == DMA1)
            RCC->AHB1ENR &= ~(1 << 21);
        else if(pDMAx == DMA2)
            RCC->AHB1ENR &= ~(1 << 22);
    }
}

/*********************************************************************
 * @fn              - DMA_Init
 * @brief           - Initializes DMA stream with specified configuration
 * @param[in]       - pDMAHandle: Pointer to DMA handle structure
 * @return          - DMA_OK or DMA_ERROR
 ********************************************************************/
DMA_StatusTypeDef DMA_Init(DMA_Handle_t *pDMAHandle)
{
    uint32_t temp = 0;
    
    if(pDMAHandle == NULL)
        return DMA_ERROR;
    
    /* Enable DMA clock */
    DMA_PeriClockControl(pDMAHandle->pDMAx, ENABLE);
    
    /* Disable stream before configuration */
    pDMAHandle->pDMAStream->CR &= ~(1 << 0);
    
    /* Wait until stream is disabled */
    while(pDMAHandle->pDMAStream->CR & (1 << 0));
    
    /* Configure Stream CR register */
    temp = pDMAHandle->pDMAStream->CR;
    
    /* Clear configuration bits */
    temp &= ~(0x7 << 25);  /* Clear channel */
    temp &= ~(0x3 << 23);  /* Clear memory burst */
    temp &= ~(0x3 << 21);  /* Clear peripheral burst */
    temp &= ~(0x3 << 16);  /* Clear priority */
    temp &= ~(0x3 << 13);  /* Clear memory data size */
    temp &= ~(0x3 << 11);  /* Clear peripheral data size */
    temp &= ~(1 << 10);    /* Clear memory increment */
    temp &= ~(1 << 9);     /* Clear peripheral increment */
    temp &= ~(1 << 8);     /* Clear circular mode */
    temp &= ~(0x3 << 6);   /* Clear direction */
    
    /* Set configuration */
    temp |= (pDMAHandle->DMA_Config.DMA_Channel << 25);
    temp |= (pDMAHandle->DMA_Config.DMA_Priority << 16);
    temp |= (pDMAHandle->DMA_Config.DMA_MemDataSize << 13);
    temp |= (pDMAHandle->DMA_Config.DMA_PeriphDataSize << 11);
    temp |= (pDMAHandle->DMA_Config.DMA_MemInc << 10);
    temp |= (pDMAHandle->DMA_Config.DMA_PeriphInc << 9);
    temp |= (pDMAHandle->DMA_Config.DMA_Mode << 8);
    temp |= (pDMAHandle->DMA_Config.DMA_Direction << 6);
    
    pDMAHandle->pDMAStream->CR = temp;
    
    /* Configure FIFO */
    if(pDMAHandle->DMA_Config.DMA_FIFOMode == DMA_FIFO_ENABLE)
    {
        pDMAHandle->pDMAStream->FCR &= ~(1 << 2);  /* Disable direct mode */
        pDMAHandle->pDMAStream->FCR &= ~(0x3 << 0); /* Clear threshold */
        pDMAHandle->pDMAStream->FCR |= (pDMAHandle->DMA_Config.DMA_FIFOThreshold << 0);
        pDMAHandle->pDMAStream->FCR |= (1 << 7);   /* Enable FIFO error interrupt */
    }
    else
    {
        pDMAHandle->pDMAStream->FCR |= (1 << 2);   /* Enable direct mode */
    }
    
    return DMA_OK;
}

/*********************************************************************
 * @fn              - DMA_DeInit
 * @brief           - Resets DMA stream to default state
 * @param[in]       - pDMAStream: Pointer to DMA stream
 ********************************************************************/
void DMA_DeInit(DMA_Stream_RegDef_t *pDMAStream)
{
    /* Disable stream */
    pDMAStream->CR &= ~(1 << 0);
    
    /* Wait until disabled */
    while(pDMAStream->CR & (1 << 0));
    
    /* Reset registers to default values */
    pDMAStream->CR = 0;
    pDMAStream->NDTR = 0;
    pDMAStream->PAR = 0;
    pDMAStream->M0AR = 0;
    pDMAStream->M1AR = 0;
    pDMAStream->FCR = 0x21;  /* Default value */
}

/*********************************************************************
 * @fn              - DMA_Start
 * @brief           - Starts DMA transfer in polling mode
 * @param[in]       - pDMAHandle: Pointer to DMA handle
 * @param[in]       - SrcAddress: Source memory address
 * @param[in]       - DstAddress: Destination memory address
 * @param[in]       - DataLength: Number of data items to transfer
 * @return          - DMA_OK, DMA_ERROR, or DMA_BUSY
 ********************************************************************/
DMA_StatusTypeDef DMA_Start(DMA_Handle_t *pDMAHandle, uint32_t SrcAddress, 
                            uint32_t DstAddress, uint16_t DataLength)
{
    if(pDMAHandle == NULL || DataLength == 0)
        return DMA_ERROR;
    
    /* Check if stream is busy */
    if(pDMAHandle->pDMAStream->CR & (1 << 0))
        return DMA_BUSY;
    
    /* Configure addresses based on direction */
    if(pDMAHandle->DMA_Config.DMA_Direction == DMA_PERIPH_TO_MEMORY)
    {
        pDMAHandle->pDMAStream->PAR = SrcAddress;   /* Peripheral address */
        pDMAHandle->pDMAStream->M0AR = DstAddress;  /* Memory address */
    }
    else if(pDMAHandle->DMA_Config.DMA_Direction == DMA_MEMORY_TO_PERIPH)
    {
        pDMAHandle->pDMAStream->PAR = DstAddress;   /* Peripheral address */
        pDMAHandle->pDMAStream->M0AR = SrcAddress;  /* Memory address */
    }
    else  /* Memory to Memory */
    {
        pDMAHandle->pDMAStream->PAR = SrcAddress;
        pDMAHandle->pDMAStream->M0AR = DstAddress;
    }
    
    /* Set number of data items */
    pDMAHandle->pDMAStream->NDTR = DataLength;
    
    /* Clear all interrupt flags */
    DMA_ClearFlag(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, 
                  DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_DME | DMA_IT_FE);
    
    /* Enable DMA stream */
    pDMAHandle->pDMAStream->CR |= (1 << 0);
    
    return DMA_OK;
}

/*********************************************************************
 * @fn              - DMA_Start_IT
 * @brief           - Starts DMA transfer in interrupt mode
 * @param[in]       - pDMAHandle: Pointer to DMA handle
 * @param[in]       - SrcAddress: Source memory address
 * @param[in]       - DstAddress: Destination memory address
 * @param[in]       - DataLength: Number of data items to transfer
 * @return          - DMA_OK, DMA_ERROR, or DMA_BUSY
 ********************************************************************/
DMA_StatusTypeDef DMA_Start_IT(DMA_Handle_t *pDMAHandle, uint32_t SrcAddress, 
                               uint32_t DstAddress, uint16_t DataLength)
{
    DMA_StatusTypeDef status;
    
    /* Start DMA transfer */
    status = DMA_Start(pDMAHandle, SrcAddress, DstAddress, DataLength);
    
    if(status != DMA_OK)
        return status;
    
    /* Enable interrupts */
    pDMAHandle->pDMAStream->CR |= (1 << 4);  /* Transfer Complete Interrupt */
    pDMAHandle->pDMAStream->CR |= (1 << 3);  /* Half Transfer Interrupt */
    pDMAHandle->pDMAStream->CR |= (1 << 2);  /* Transfer Error Interrupt */
    pDMAHandle->pDMAStream->CR |= (1 << 1);  /* Direct Mode Error Interrupt */
    
    return DMA_OK;
}

/*********************************************************************
 * @fn              - DMA_Stop
 * @brief           - Stops DMA transfer
 * @param[in]       - pDMAStream: Pointer to DMA stream
 ********************************************************************/
void DMA_Stop(DMA_Stream_RegDef_t *pDMAStream)
{
    /* Disable stream */
    pDMAStream->CR &= ~(1 << 0);
    
    /* Wait until stream is disabled */
    while(pDMAStream->CR & (1 << 0));
}

/*********************************************************************
 * @fn              - DMA_GetFlagStatus
 * @brief           - Gets DMA interrupt flag status
 * @param[in]       - pDMAx: DMA1 or DMA2
 * @param[in]       - StreamNumber: Stream number (0-7)
 * @param[in]       - Flag: Flag to check
 * @return          - FLAG_SET or FLAG_RESET
 ********************************************************************/
uint8_t DMA_GetFlagStatus(DMA_RegDef_t *pDMAx, uint8_t StreamNumber, uint32_t Flag)
{
    uint32_t status_reg;
    uint8_t bit_offset;
    
    /* Calculate bit offset for the stream */
    if(StreamNumber < 4)
    {
        bit_offset = StreamNumber * 6;
        if(StreamNumber == 1 || StreamNumber == 3)
            bit_offset += 4;
        
        status_reg = pDMAx->LISR;
    }
    else
    {
        bit_offset = (StreamNumber - 4) * 6;
        if(StreamNumber == 5 || StreamNumber == 7)
            bit_offset += 4;
        
        status_reg = pDMAx->HISR;
    }
    
    if(status_reg & (Flag << bit_offset))
        return FLAG_SET;
    
    return FLAG_RESET;
}

/*********************************************************************
 * @fn              - DMA_ClearFlag
 * @brief           - Clears DMA interrupt flags
 * @param[in]       - pDMAx: DMA1 or DMA2
 * @param[in]       - StreamNumber: Stream number (0-7)
 * @param[in]       - Flag: Flag(s) to clear
 ********************************************************************/
void DMA_ClearFlag(DMA_RegDef_t *pDMAx, uint8_t StreamNumber, uint32_t Flag)
{
    uint8_t bit_offset;
    
    /* Calculate bit offset */
    if(StreamNumber < 4)
    {
        bit_offset = StreamNumber * 6;
        if(StreamNumber == 1 || StreamNumber == 3)
            bit_offset += 4;
        
        pDMAx->LIFCR = (Flag << bit_offset);
    }
    else
    {
        bit_offset = (StreamNumber - 4) * 6;
        if(StreamNumber == 5 || StreamNumber == 7)
            bit_offset += 4;
        
        pDMAx->HIFCR = (Flag << bit_offset);
    }
}

/*********************************************************************
 * @fn              - DMA_IRQInterruptConfig
 * @brief           - Configures DMA interrupt in NVIC
 * @param[in]       - IRQNumber: IRQ number
 * @param[in]       - EnorDi: ENABLE or DISABLE
 ********************************************************************/
void DMA_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi)
{
    if(EnorDi == ENABLE)
    {
        if(IRQNumber <= 31)
            *NVIC_ISER0 |= (1 << IRQNumber);
        else if(IRQNumber > 31 && IRQNumber < 64)
            *NVIC_ISER1 |= (1 << (IRQNumber % 32));
        else if(IRQNumber >= 64 && IRQNumber < 96)
            *NVIC_ISER2 |= (1 << (IRQNumber % 64));
    }
    else
    {
        if(IRQNumber <= 31)
            *NVIC_ICER0 |= (1 << IRQNumber);
        else if(IRQNumber > 31 && IRQNumber < 64)
            *NVIC_ICER1 |= (1 << (IRQNumber % 32));
        else if(IRQNumber >= 64 && IRQNumber < 96)
            *NVIC_ICER2 |= (1 << (IRQNumber % 64));
    }
}

/*********************************************************************
 * @fn              - DMA_IRQPriorityConfig
 * @brief           - Configures DMA interrupt priority
 * @param[in]       - IRQNumber: IRQ number
 * @param[in]       - IRQPriority: Priority value
 ********************************************************************/
void DMA_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority)
{
    uint8_t iprx = IRQNumber / 4;
    uint8_t iprx_section = IRQNumber % 4;
    uint8_t shift_amount = (8 * iprx_section) + (8 - NO_PR_BITS_IMPLEMENTED);
    
    *(NVIC_PR_BASE_ADDR + iprx) &= ~(0xFF << shift_amount);
    *(NVIC_PR_BASE_ADDR + iprx) |= (IRQPriority << shift_amount);
}

/*********************************************************************
 * @fn              - DMA_IRQHandling
 * @brief           - Handles DMA interrupts
 * @param[in]       - pDMAHandle: Pointer to DMA handle
 ********************************************************************/
void DMA_IRQHandling(DMA_Handle_t *pDMAHandle)
{
    /* Transfer Complete */
    if(DMA_GetFlagStatus(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_TC))
    {
        DMA_ClearFlag(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_TC);
        DMA_TransferCompleteCallback(pDMAHandle);
    }
    
    /* Half Transfer */
    if(DMA_GetFlagStatus(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_HT))
    {
        DMA_ClearFlag(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_HT);
        DMA_HalfTransferCallback(pDMAHandle);
    }
    
    /* Transfer Error */
    if(DMA_GetFlagStatus(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_TE))
    {
        DMA_ClearFlag(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_TE);
        DMA_TransferErrorCallback(pDMAHandle);
    }
    
    /* Direct Mode Error */
    if(DMA_GetFlagStatus(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_DME))
    {
        DMA_ClearFlag(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_DME);
    }
    
    /* FIFO Error */
    if(DMA_GetFlagStatus(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_FE))
    {
        DMA_ClearFlag(pDMAHandle->pDMAx, pDMAHandle->StreamNumber, DMA_IT_FE);
    }
}

/*********************************************************************
 * @fn              - DMA_GetStreamNumber
 * @brief           - Gets stream number from stream pointer
 * @param[in]       - pDMAStream: Pointer to DMA stream
 * @return          - Stream number (0-7)
 ********************************************************************/
uint8_t DMA_GetStreamNumber(DMA_Stream_RegDef_t *pDMAStream)
{
    uint32_t base_addr = (uint32_t)pDMAStream;
    uint32_t dma1_base = DMA1_BASEADDR;
    uint32_t dma2_base = DMA2_BASEADDR;
    
    if(base_addr >= dma1_base && base_addr < dma1_base + 0x400)
        return ((base_addr - dma1_base - 0x10) / 0x18);
    else
        return ((base_addr - dma2_base - 0x10) / 0x18);
}

/*********************************************************************
 * @fn              - DMA_GetController
 * @brief           - Gets DMA controller from stream pointer
 * @param[in]       - pDMAStream: Pointer to DMA stream
 * @return          - Pointer to DMA controller (DMA1 or DMA2)
 ********************************************************************/
DMA_RegDef_t* DMA_GetController(DMA_Stream_RegDef_t *pDMAStream)
{
    uint32_t base_addr = (uint32_t)pDMAStream;
    
    if(base_addr >= DMA1_BASEADDR && base_addr < DMA1_BASEADDR + 0x400)
        return DMA1;
    else
        return DMA2;
}

/*********************************************************************
 * Weak Callback Functions - Can be overridden by user application
 ********************************************************************/
__weak void DMA_TransferCompleteCallback(DMA_Handle_t *pDMAHandle)
{
    /* User application should override this function */
}

__weak void DMA_TransferErrorCallback(DMA_Handle_t *pDMAHandle)
{
    /* User application should override this function */
}

__weak void DMA_HalfTransferCallback(DMA_Handle_t *pDMAHandle)
{
    /* User application should override this function */
}
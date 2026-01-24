/*
 * 019_DMA_Examples.c
 *
 *  Created on: Jan 18, 2026
 *      Author: Rahul B.
 *  Description: Complete DMA usage examples for STM32F446RE
 */

#include "stm32f446xx.h"
#include "stm32f446xx_dma_driver.h"
#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_usart_driver.h"
#include "stm32f446xx_timer_driver.h"

/* Global Variables */
uint8_t transfer_complete_flag = 0;

void gpio_led_init(void)
{
    GPIO_Handle_t GpioLed;
    GpioLed.pGPIOx = GPIOA;
    GpioLed.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_5;
    GpioLed.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_OUT;
    GpioLed.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
    GpioLed.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_PP;
    GpioLed.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
    GPIO_Init(&GpioLed);
}

/* ============================================================================
   EXAMPLE 1: Memory to Memory Transfer (Polling Mode)
   ============================================================================ */
void Example1_MemToMem_Polling(void)
{
    DMA_Handle_t hdma;
    uint32_t src_buffer[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    uint32_t dst_buffer[10] = {0};
    
    TIMER_DelayInit();
    gpio_led_init();
    
    /* Configure DMA */
    hdma.pDMAStream = DMA2_Stream0;
    hdma.pDMAx = DMA2;
    hdma.StreamNumber = 0;
    
    hdma.DMA_Config.DMA_Channel = DMA_CHANNEL_0;
    hdma.DMA_Config.DMA_Direction = DMA_MEMORY_TO_MEMORY;
    hdma.DMA_Config.DMA_PeriphDataSize = DMA_SIZE_WORD;
    hdma.DMA_Config.DMA_MemDataSize = DMA_SIZE_WORD;
    hdma.DMA_Config.DMA_Mode = DMA_MODE_NORMAL;
    hdma.DMA_Config.DMA_Priority = DMA_PRIORITY_HIGH;
    hdma.DMA_Config.DMA_PeriphInc = DMA_PINC_ENABLE;
    hdma.DMA_Config.DMA_MemInc = DMA_MINC_ENABLE;
    hdma.DMA_Config.DMA_FIFOMode = DMA_FIFO_DISABLE;
    
    /* Initialize DMA */
    DMA_Init(&hdma);
    
    /* Start DMA transfer */
    DMA_Start(&hdma, (uint32_t)src_buffer, (uint32_t)dst_buffer, 10);
    
    /* Wait for transfer complete */
    while(DMA_GetFlagStatus(DMA2, 0, DMA_IT_TC) == FLAG_RESET);
    
    /* Clear flag */
    DMA_ClearFlag(DMA2, 0, DMA_IT_TC);
    
    /* Blink LED to indicate success */
    for(int i = 0; i < 5; i++)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 500);
    }
    
    while(1);
}

/* ============================================================================
   EXAMPLE 2: Memory to Memory Transfer (Interrupt Mode)
   ============================================================================ */
void Example2_MemToMem_Interrupt(void)
{
    DMA_Handle_t hdma;
    uint8_t src_buffer[100];
    uint8_t dst_buffer[100];
    
    /* Fill source buffer */
    for(int i = 0; i < 100; i++)
        src_buffer[i] = i;
    
    TIMER_DelayInit();
    gpio_led_init();
    
    /* Configure DMA */
    hdma.pDMAStream = DMA2_Stream0;
    hdma.pDMAx = DMA2;
    hdma.StreamNumber = 0;
    
    hdma.DMA_Config.DMA_Channel = DMA_CHANNEL_0;
    hdma.DMA_Config.DMA_Direction = DMA_MEMORY_TO_MEMORY;
    hdma.DMA_Config.DMA_PeriphDataSize = DMA_SIZE_BYTE;
    hdma.DMA_Config.DMA_MemDataSize = DMA_SIZE_BYTE;
    hdma.DMA_Config.DMA_Mode = DMA_MODE_NORMAL;
    hdma.DMA_Config.DMA_Priority = DMA_PRIORITY_VERY_HIGH;
    hdma.DMA_Config.DMA_PeriphInc = DMA_PINC_ENABLE;
    hdma.DMA_Config.DMA_MemInc = DMA_MINC_ENABLE;
    hdma.DMA_Config.DMA_FIFOMode = DMA_FIFO_DISABLE;
    
    DMA_Init(&hdma);
    
    /* Configure NVIC */
    DMA_IRQInterruptConfig(DMA2_Stream0_IRQn, ENABLE);
    DMA_IRQPriorityConfig(DMA2_Stream0_IRQn, NVIC_IRQ_PRI15);
    
    /* Start DMA transfer with interrupt */
    DMA_Start_IT(&hdma, (uint32_t)src_buffer, (uint32_t)dst_buffer, 100);
    
    /* Wait for transfer complete flag */
    while(transfer_complete_flag == 0);
    
    /* Blink LED rapidly */
    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 200);
    }
}

/* ============================================================================
   EXAMPLE 3: DMA with USART TX (Interrupt Mode)
   ============================================================================ */
void Example3_DMA_USART_TX(void)
{
    DMA_Handle_t hdma;
    USART_Handle_t husart;
    char message[] = "Hello from DMA-USART!\r\n";
    
    TIMER_DelayInit();
    
    /* Configure USART2 */
    husart.pUSARTx = USART2;
    husart.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
    husart.USART_Config.USART_Baud = USART_STD_BAUD_115200;
    husart.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1;
    husart.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
    husart.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
    husart.USART_Config.USART_HWFlowControl = USART_HW_FLOW_CTRL_NONE;
    
    USART_Init(&husart);
    USART_PeripheralControl(USART2, ENABLE);
    
    /* Configure DMA for USART2 TX (DMA1 Stream6 Channel 4) */
    hdma.pDMAStream = DMA1_Stream6;
    hdma.pDMAx = DMA1;
    hdma.StreamNumber = 6;
    
    hdma.DMA_Config.DMA_Channel = DMA_CHANNEL_4;
    hdma.DMA_Config.DMA_Direction = DMA_MEMORY_TO_PERIPH;
    hdma.DMA_Config.DMA_PeriphDataSize = DMA_SIZE_BYTE;
    hdma.DMA_Config.DMA_MemDataSize = DMA_SIZE_BYTE;
    hdma.DMA_Config.DMA_Mode = DMA_MODE_NORMAL;
    hdma.DMA_Config.DMA_Priority = DMA_PRIORITY_HIGH;
    hdma.DMA_Config.DMA_PeriphInc = DMA_PINC_DISABLE;
    hdma.DMA_Config.DMA_MemInc = DMA_MINC_ENABLE;
    hdma.DMA_Config.DMA_FIFOMode = DMA_FIFO_DISABLE;
    
    DMA_Init(&hdma);
    
    /* Enable USART DMA TX */
    USART2->CR3 |= (1 << 7);  /* DMAT bit */
    
    /* Configure NVIC */
    DMA_IRQInterruptConfig(DMA1_Stream6_IRQn, ENABLE);
    DMA_IRQPriorityConfig(DMA1_Stream6_IRQn, NVIC_IRQ_PRI15);
    
    /* Start DMA transfer */
    DMA_Start_IT(&hdma, (uint32_t)message, (uint32_t)&USART2->DR, sizeof(message)-1);
    
    /* Wait for completion */
    while(transfer_complete_flag == 0);
    
    while(1);
}

/* ============================================================================
   EXAMPLE 4: Circular DMA Buffer for ADC
   ============================================================================ */
void Example4_DMA_Circular_ADC(void)
{
    DMA_Handle_t hdma;
    uint16_t adc_buffer[100];
    
    TIMER_DelayInit();
    gpio_led_init();
    
    /* Configure DMA for ADC (DMA2 Stream0 Channel 0) */
    hdma.pDMAStream = DMA2_Stream0;
    hdma.pDMAx = DMA2;
    hdma.StreamNumber = 0;
    
    hdma.DMA_Config.DMA_Channel = DMA_CHANNEL_0;
    hdma.DMA_Config.DMA_Direction = DMA_PERIPH_TO_MEMORY;
    hdma.DMA_Config.DMA_PeriphDataSize = DMA_SIZE_HALFWORD;
    hdma.DMA_Config.DMA_MemDataSize = DMA_SIZE_HALFWORD;
    hdma.DMA_Config.DMA_Mode = DMA_MODE_CIRCULAR;  /* Circular mode */
    hdma.DMA_Config.DMA_Priority = DMA_PRIORITY_HIGH;
    hdma.DMA_Config.DMA_PeriphInc = DMA_PINC_DISABLE;
    hdma.DMA_Config.DMA_MemInc = DMA_MINC_ENABLE;
    hdma.DMA_Config.DMA_FIFOMode = DMA_FIFO_DISABLE;
    
    DMA_Init(&hdma);
    
    /* Configure NVIC */
    DMA_IRQInterruptConfig(DMA2_Stream0_IRQn, ENABLE);
    DMA_IRQPriorityConfig(DMA2_Stream0_IRQn, NVIC_IRQ_PRI15);
    
    /* Start circular DMA */
    DMA_Start_IT(&hdma, (uint32_t)&ADC1->DR, (uint32_t)adc_buffer, 100);
    
    while(1)
    {
        /* Process ADC data here */
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 1000);
    }
}

/* ============================================================================
   EXAMPLE 5: Double Buffer Mode
   ============================================================================ */
void Example5_DMA_DoubleBuffer(void)
{
    DMA_Handle_t hdma;
    uint32_t buffer0[50];
    uint32_t buffer1[50];
    
    TIMER_DelayInit();
    
    /* Configure DMA with double buffer */
    hdma.pDMAStream = DMA2_Stream0;
    hdma.pDMAx = DMA2;
    hdma.StreamNumber = 0;
    
    hdma.DMA_Config.DMA_Channel = DMA_CHANNEL_0;
    hdma.DMA_Config.DMA_Direction = DMA_MEMORY_TO_MEMORY;
    hdma.DMA_Config.DMA_PeriphDataSize = DMA_SIZE_WORD;
    hdma.DMA_Config.DMA_MemDataSize = DMA_SIZE_WORD;
    hdma.DMA_Config.DMA_Mode = DMA_MODE_CIRCULAR;
    hdma.DMA_Config.DMA_Priority = DMA_PRIORITY_VERY_HIGH;
    hdma.DMA_Config.DMA_PeriphInc = DMA_PINC_ENABLE;
    hdma.DMA_Config.DMA_MemInc = DMA_MINC_ENABLE;
    hdma.DMA_Config.DMA_FIFOMode = DMA_FIFO_ENABLE;
    hdma.DMA_Config.DMA_FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    
    DMA_Init(&hdma);
    
    /* Enable double buffer mode */
    hdma.pDMAStream->CR |= (1 << 18);  /* DBM bit */
    hdma.pDMAStream->M0AR = (uint32_t)buffer0;
    hdma.pDMAStream->M1AR = (uint32_t)buffer1;
    
    /* Start DMA */
    DMA_Start_IT(&hdma, 0, 0, 50);
    
    while(1);
}

/* ============================================================================
   DMA Interrupt Handlers
   ============================================================================ */
void DMA1_Stream6_IRQHandler(void)
{
    DMA_Handle_t hdma;
    hdma.pDMAx = DMA1;
    hdma.pDMAStream = DMA1_Stream6;
    hdma.StreamNumber = 6;
    
    DMA_IRQHandling(&hdma);
}

void DMA2_Stream0_IRQHandler(void)
{
    DMA_Handle_t hdma;
    hdma.pDMAx = DMA2;
    hdma.pDMAStream = DMA2_Stream0;
    hdma.StreamNumber = 0;
    
    DMA_IRQHandling(&hdma);
}

/* ============================================================================
   DMA Callbacks
   ============================================================================ */
void DMA_TransferCompleteCallback(DMA_Handle_t *pDMAHandle)
{
    transfer_complete_flag = 1;
    /* Toggle LED on transfer complete */
    GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
}

void DMA_TransferErrorCallback(DMA_Handle_t *pDMAHandle)
{
    /* Blink LED rapidly on error */
    while(1)
    {
        GPIO_ToggleOutputPin(GPIOA, GPIO_PIN_NO_5);
        TIMER_DelayMs(TIM2, 100);
    }
}

void DMA_HalfTransferCallback(DMA_Handle_t *pDMAHandle)
{
    /* Process first half of buffer */
}

/* ============================================================================
   MAIN FUNCTION - Select which example to run
   ============================================================================ */
int main(void)
{
    SystemClock_Config_HSE_180MHz();
    
    /* Uncomment the example you want to run */
    
    Example1_MemToMem_Polling();        /* Memory to Memory (Polling) */
    // Example2_MemToMem_Interrupt();   /* Memory to Memory (Interrupt) */
    // Example3_DMA_USART_TX();         /* DMA with USART TX */
    // Example4_DMA_Circular_ADC();     /* Circular DMA for ADC */
    // Example5_DMA_DoubleBuffer();     /* Double Buffer Mode */
    
    return 0;
}
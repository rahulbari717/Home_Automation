/*
 * stm32f446xx_dma_driver.h
 *
 *  Created on: Jan 21, 2026
 *      Author: Rahul B.
 */

#ifndef INC_STM32F446XX_DMA_DRIVER_H_
#define INC_STM32F446XX_DMA_DRIVER_H_

#include "stm32f446xx.h"

/*
 * DMA Register Definitions
 */
typedef struct
{
    volatile uint32_t CR;       /* Stream configuration register */
    volatile uint32_t NDTR;     /* Number of data register */
    volatile uint32_t PAR;      /* Peripheral address register */
    volatile uint32_t M0AR;     /* Memory 0 address register */
    volatile uint32_t M1AR;     /* Memory 1 address register */
    volatile uint32_t FCR;      /* FIFO control register */
} DMA_Stream_RegDef_t;

typedef struct
{
    volatile uint32_t LISR;     /* Low interrupt status register */
    volatile uint32_t HISR;     /* High interrupt status register */
    volatile uint32_t LIFCR;    /* Low interrupt flag clear register */
    volatile uint32_t HIFCR;    /* High interrupt flag clear register */
} DMA_RegDef_t;

/*
 * DMA Peripheral Definitions
 */
#define DMA1                    ((DMA_RegDef_t*)DMA1_BASEADDR)
#define DMA2                    ((DMA_RegDef_t*)DMA2_BASEADDR)

#define DMA1_Stream0            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0x10))
#define DMA1_Stream1            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0x28))
#define DMA1_Stream2            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0x40))
#define DMA1_Stream3            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0x58))
#define DMA1_Stream4            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0x70))
#define DMA1_Stream5            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0x88))
#define DMA1_Stream6            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0xA0))
#define DMA1_Stream7            ((DMA_Stream_RegDef_t*)(DMA1_BASEADDR + 0xB8))

#define DMA2_Stream0            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0x10))
#define DMA2_Stream1            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0x28))
#define DMA2_Stream2            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0x40))
#define DMA2_Stream3            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0x58))
#define DMA2_Stream4            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0x70))
#define DMA2_Stream5            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0x88))
#define DMA2_Stream6            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0xA0))
#define DMA2_Stream7            ((DMA_Stream_RegDef_t*)(DMA2_BASEADDR + 0xB8))

/*
 * @DMA_CHANNEL
 * DMA Channel Selection (0-7)
 */
#define DMA_CHANNEL_0           0
#define DMA_CHANNEL_1           1
#define DMA_CHANNEL_2           2
#define DMA_CHANNEL_3           3
#define DMA_CHANNEL_4           4
#define DMA_CHANNEL_5           5
#define DMA_CHANNEL_6           6
#define DMA_CHANNEL_7           7

/*
 * @DMA_DIRECTION
 * DMA Transfer Direction
 */
#define DMA_PERIPH_TO_MEMORY    0       /* Peripheral to Memory */
#define DMA_MEMORY_TO_PERIPH    1       /* Memory to Peripheral */
#define DMA_MEMORY_TO_MEMORY    2       /* Memory to Memory */

/*
 * @DMA_DATA_SIZE
 * DMA Data Size
 */
#define DMA_SIZE_BYTE           0       /* 8-bit data */
#define DMA_SIZE_HALFWORD       1       /* 16-bit data */
#define DMA_SIZE_WORD           2       /* 32-bit data */

/*
 * @DMA_MODE
 * DMA Mode
 */
#define DMA_MODE_NORMAL         0       /* Normal mode */
#define DMA_MODE_CIRCULAR       1       /* Circular mode */

/*
 * @DMA_PRIORITY
 * DMA Priority Level
 */
#define DMA_PRIORITY_LOW        0
#define DMA_PRIORITY_MEDIUM     1
#define DMA_PRIORITY_HIGH       2
#define DMA_PRIORITY_VERY_HIGH  3

/*
 * @DMA_INCREMENT
 * DMA Address Increment
 */
#define DMA_PINC_DISABLE        0       /* Peripheral increment disabled */
#define DMA_PINC_ENABLE         1       /* Peripheral increment enabled */
#define DMA_MINC_DISABLE        0       /* Memory increment disabled */
#define DMA_MINC_ENABLE         1       /* Memory increment enabled */

/*
 * @DMA_FIFO_MODE
 * DMA FIFO Mode
 */
#define DMA_FIFO_DISABLE        0       /* Direct mode */
#define DMA_FIFO_ENABLE         1       /* FIFO mode */

/*
 * @DMA_FIFO_THRESHOLD
 * DMA FIFO Threshold
 */
#define DMA_FIFO_THRESHOLD_1_4  0
#define DMA_FIFO_THRESHOLD_1_2  1
#define DMA_FIFO_THRESHOLD_3_4  2
#define DMA_FIFO_THRESHOLD_FULL 3

/*
 * DMA Interrupt Flags
 */
#define DMA_IT_TC               (1 << 5)    /* Transfer Complete */
#define DMA_IT_HT               (1 << 4)    /* Half Transfer */
#define DMA_IT_TE               (1 << 3)    /* Transfer Error */
#define DMA_IT_DME              (1 << 2)    /* Direct Mode Error */
#define DMA_IT_FE               (1 << 0)    /* FIFO Error */

/*
 * DMA Configuration Structure
 */
typedef struct
{
    uint8_t DMA_Channel;            /* Channel selection @DMA_CHANNEL */
    uint8_t DMA_Direction;          /* Transfer direction @DMA_DIRECTION */
    uint8_t DMA_PeriphDataSize;     /* Peripheral data size @DMA_DATA_SIZE */
    uint8_t DMA_MemDataSize;        /* Memory data size @DMA_DATA_SIZE */
    uint8_t DMA_Mode;               /* DMA mode @DMA_MODE */
    uint8_t DMA_Priority;           /* Priority level @DMA_PRIORITY */
    uint8_t DMA_PeriphInc;          /* Peripheral increment @DMA_INCREMENT */
    uint8_t DMA_MemInc;             /* Memory increment @DMA_INCREMENT */
    uint8_t DMA_FIFOMode;           /* FIFO mode @DMA_FIFO_MODE */
    uint8_t DMA_FIFOThreshold;      /* FIFO threshold @DMA_FIFO_THRESHOLD */
} DMA_Config_t;

/*
 * DMA Handle Structure
 */
typedef struct
{
    DMA_Stream_RegDef_t *pDMAStream;    /* DMA Stream base address */
    DMA_RegDef_t *pDMAx;                /* DMA controller (DMA1 or DMA2) */
    DMA_Config_t DMA_Config;            /* DMA configuration */
    uint8_t StreamNumber;               /* Stream number (0-7) */
} DMA_Handle_t;

/*
 * DMA Status
 */
typedef enum
{
    DMA_OK      = 0x00U,
    DMA_ERROR   = 0x01U,
    DMA_BUSY    = 0x02U,
    DMA_TIMEOUT = 0x03U
} DMA_StatusTypeDef;

/******************************************************************************************
 *                              APIs supported by this driver
 ******************************************************************************************/

/*
 * DMA Clock Control
 */
void DMA_PeriClockControl(DMA_RegDef_t *pDMAx, uint8_t EnorDi);

/*
 * DMA Init and De-init
 */
DMA_StatusTypeDef DMA_Init(DMA_Handle_t *pDMAHandle);
void DMA_DeInit(DMA_Stream_RegDef_t *pDMAStream);

/*
 * DMA Transfer Functions
 */
DMA_StatusTypeDef DMA_Start(DMA_Handle_t *pDMAHandle, uint32_t SrcAddress, 
                            uint32_t DstAddress, uint16_t DataLength);
void DMA_Stop(DMA_Stream_RegDef_t *pDMAStream);

/*
 * DMA Interrupt Functions
 */
DMA_StatusTypeDef DMA_Start_IT(DMA_Handle_t *pDMAHandle, uint32_t SrcAddress, 
                               uint32_t DstAddress, uint16_t DataLength);
void DMA_IRQInterruptConfig(uint8_t IRQNumber, uint8_t EnorDi);
void DMA_IRQPriorityConfig(uint8_t IRQNumber, uint32_t IRQPriority);
void DMA_IRQHandling(DMA_Handle_t *pDMAHandle);

/*
 * DMA Status and Flag Functions
 */
uint8_t DMA_GetFlagStatus(DMA_RegDef_t *pDMAx, uint8_t StreamNumber, uint32_t Flag);
void DMA_ClearFlag(DMA_RegDef_t *pDMAx, uint8_t StreamNumber, uint32_t Flag);

/*
 * Helper Functions
 */
uint8_t DMA_GetStreamNumber(DMA_Stream_RegDef_t *pDMAStream);
DMA_RegDef_t* DMA_GetController(DMA_Stream_RegDef_t *pDMAStream);

/*
 * Application Callback
 */
void DMA_TransferCompleteCallback(DMA_Handle_t *pDMAHandle);
void DMA_TransferErrorCallback(DMA_Handle_t *pDMAHandle);
void DMA_HalfTransferCallback(DMA_Handle_t *pDMAHandle);

#endif /* INC_STM32F446XX_DMA_DRIVER_H_ */
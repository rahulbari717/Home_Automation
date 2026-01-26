#ifndef INC_BSP_DELAY_H_
#define INC_BSP_DELAY_H_

#include "stm32f446xx_timer_driver.h"

/* * Initialize the delay timer (TIM2)
 * Must be called once during system startup.
 */
void BSP_Delay_Init(void);

/* Generic delay function in milliseconds */
void BSP_Delay_ms(uint32_t ms);

/* Specific fixed-duration delay functions */
void BSP_Delay_100ms(void);
void BSP_Delay_500ms(void);
void BSP_Delay_1s(void);
void BSP_Delay_3s(void);


/* Global status flag (0 = Busy, 1 = Complete) */
extern volatile uint8_t g_BSP_DelayComplete;

/* Initialization */
void BSP_Delay_Init_IT(void);

/* Non-blocking API - Use these to start the timer */
void BSP_DelayUs_IT(uint32_t microseconds);
void BSP_DelayMs_IT(uint32_t milliseconds);
void BSP_DelaySec_IT(uint32_t seconds);

/* Check status helper */
uint8_t BSP_Delay_IsComplete(void);

#endif /* INC_BSP_DELAY_H_ */
